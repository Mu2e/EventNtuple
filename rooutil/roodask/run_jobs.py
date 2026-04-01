#!/usr/bin/env python3
"""
run_jobs.py — Dask Distributed C++ Job Runner

Compiles a C++ ROOT macro into a standalone binary, then distributes jobs
across Dask workers.  Input files are read from a filelist and split into
batches of --files-per-job files each.

Usage:
    # Compile and run locally (1 file per job, uses all CPUs)
    python run_jobs.py --manifest jobs.json --filelist files.txt

    # 5 files per job, 4 workers
    python run_jobs.py --manifest jobs.json --filelist files.txt --files-per-job 5 --n-workers 4

    # Skip compilation (reuse previously compiled binary)
    python run_jobs.py --manifest jobs.json --filelist files.txt --skip-compile

    # Connect to an existing scheduler
    python run_jobs.py --manifest jobs.json --filelist files.txt --scheduler tcp://host:8786
"""

from __future__ import annotations

import argparse
import json
import os
import shlex
import subprocess
import sys
import time
from pathlib import Path

from dask.distributed import Client, LocalCluster, as_completed


# ── Environment variable expansion ──────────────────────────────────

def expand_env_vars(obj):
    """Recursively expand ${VAR} environment variables in all strings."""
    if isinstance(obj, str):
        return os.path.expandvars(obj)
    if isinstance(obj, dict):
        return {k: expand_env_vars(v) for k, v in obj.items()}
    if isinstance(obj, list):
        return [expand_env_vars(item) for item in obj]
    return obj


# ── Compilation ─────────────────────────────────────────────────────

def compile_source(
    source: Path,
    output_binary: Path,
    include_dirs: list[str] | None = None,
    libraries: list[str] | None = None,
    compile_flags: list[str] | None = None,
) -> Path:
    """Compile a C++ ROOT macro into a standalone binary.

    Auto-generates a main() wrapper that includes the macro and calls
    its entry-point function with argv[1] (input) and argv[2] (output).

    Returns the path to the compiled binary.
    Raises RuntimeError on compilation failure.
    """
    # Get ROOT compiler/linker flags
    try:
        cflags = subprocess.run(
            ["root-config", "--cflags"],
            capture_output=True, text=True, check=True,
        ).stdout.strip()
        libs = subprocess.run(
            ["root-config", "--libs"],
            capture_output=True, text=True, check=True,
        ).stdout.strip()
    except FileNotFoundError:
        raise RuntimeError(
            "root-config not found. Ensure ROOT is set up in your environment."
        )

    # Parse the macro to find the entry-point function name
    # Convention: function name matches the file stem (ROOT macro style)
    func_name = source.stem

    # Generate a main() wrapper that calls the macro function
    work_dir = output_binary.parent
    work_dir.mkdir(parents=True, exist_ok=True)
    wrapper_path = work_dir / f"{func_name}_main.cpp"
    wrapper_path.write_text(
        f'#include "{source.resolve()}"\n'
        f"\n"
        f"int main(int argc, char* argv[]) {{\n"
        f'  if (argc < 3) {{ std::cerr << "Usage: " << argv[0] << " <input> <output>" << std::endl; return 1; }}\n'
        f"  {func_name}(argv[1], argv[2]);\n"
        f"  return 0;\n"
        f"}}\n"
    )
    print(f"Generated wrapper: {wrapper_path}")

    # Use $CXX if set, otherwise g++ from the user's PATH
    compiler = os.environ.get("CXX", "g++")

    # Print compiler version
    gpp_version = subprocess.run(
        [compiler, "--version"], capture_output=True, text=True,
    ).stdout.splitlines()[0]
    print(f"Compiler: {compiler}")
    print(f"Version:  {gpp_version}")

    # Build the compile command (compile the wrapper, not the macro directly)
    cmd_parts = [compiler, "-o", str(output_binary), str(wrapper_path)]
    cmd_parts += shlex.split(cflags)

    # Use RUNPATH (not RPATH) so LD_LIBRARY_PATH takes priority at runtime.
    # GCC's specs file embeds its lib64 as DT_RPATH which overrides
    # LD_LIBRARY_PATH; --enable-new-dtags converts all rpaths to RUNPATH.
    cmd_parts.append("-Wl,--enable-new-dtags")

    # Add library search paths from LD_LIBRARY_PATH BEFORE root-config libs
    # so the correct libstdc++/libtbb are found first at link time.
    for ld_path in os.environ.get("LD_LIBRARY_PATH", "").split(os.pathsep):
        if ld_path:
            cmd_parts.append(f"-L{ld_path}")
            cmd_parts.append(f"-Wl,-rpath,{ld_path}")

    cmd_parts += shlex.split(libs)

    # ROOT's libImt requires TBB but root-config doesn't include it
    cmd_parts.append("-ltbb")

    # Add the macro's directory as an include path
    cmd_parts.append(f"-I{source.resolve().parent}")

    for d in (include_dirs or []):
        # Split colon-separated paths (e.g. "${SOME_PATH}" → "/a:/b:/c")
        for p in d.split(os.pathsep):
            if p:
                cmd_parts.append(f"-I{p}")
    for lib in (libraries or []):
        cmd_parts.append(f"-l{lib}")
    cmd_parts += (compile_flags or [])

    print(f"Compiling: {' '.join(cmd_parts)}")
    t0 = time.monotonic()

    result = subprocess.run(
        cmd_parts, capture_output=True, text=True,
    )
    elapsed = time.monotonic() - t0

    if result.returncode != 0:
        print("COMPILATION FAILED", file=sys.stderr)
        print(result.stdout, file=sys.stderr)
        print(result.stderr, file=sys.stderr)
        raise RuntimeError(f"Compilation failed (exit code {result.returncode})")

    print(f"Compiled successfully in {elapsed:.1f}s → {output_binary}")
    return output_binary


# ── Task function (runs on each worker) ─────────────────────────────

def run_cpp_job(
    job: dict,
    binary: str,
    work_dir: str,
    timeout: int,
    env: dict | None = None,
) -> dict:
    """Run a single C++ job: write batch filelist, execute binary, capture output."""

    job_id = job["id"]
    files = job["files"]
    output_file = job["output_file"]

    # Ensure per-job directories exist
    work_path = Path(work_dir)
    work_path.mkdir(parents=True, exist_ok=True)
    out_dir = Path(output_file).parent
    out_dir.mkdir(parents=True, exist_ok=True)

    # Write batch filelist (one file path per line)
    filelist_path = work_path / f"{job_id}_filelist.txt"
    filelist_path.write_text("\n".join(files) + "\n")

    # Run the binary: binary <filelist> <output>
    t0 = time.monotonic()
    try:
        result = subprocess.run(
            [binary, str(filelist_path), output_file],
            capture_output=True,
            text=True,
            timeout=timeout,
            env=env,
        )
        elapsed = time.monotonic() - t0
        return {
            "job_id": job_id,
            "success": result.returncode == 0,
            "returncode": result.returncode,
            "stdout": result.stdout,
            "stderr": result.stderr,
            "elapsed_seconds": round(elapsed, 2),
            "n_files": len(files),
            "filelist": str(filelist_path),
            "output_file": output_file,
        }
    except subprocess.TimeoutExpired:
        elapsed = time.monotonic() - t0
        return {
            "job_id": job_id,
            "success": False,
            "returncode": None,
            "stdout": "",
            "stderr": f"TIMEOUT after {timeout}s",
            "elapsed_seconds": round(elapsed, 2),
            "n_files": len(files),
            "filelist": str(filelist_path),
            "output_file": output_file,
        }
    except Exception as exc:
        elapsed = time.monotonic() - t0
        return {
            "job_id": job_id,
            "success": False,
            "returncode": None,
            "stdout": "",
            "stderr": str(exc),
            "elapsed_seconds": round(elapsed, 2),
            "n_files": len(files),
            "filelist": str(filelist_path),
            "output_file": output_file,
        }


# ── CLI & main ───────────────────────────────────────────────────────

def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description="Compile and run a C++ ROOT macro across Dask workers",
    )
    p.add_argument(
        "--manifest", required=True,
        help="Path to jobs.json manifest file",
    )
    p.add_argument(
        "--filelist", required=True,
        help="Path to a text file with one input file per line",
    )
    p.add_argument(
        "--files-per-job", type=int, default=1,
        help="Number of input files per job (default: 1)",
    )
    p.add_argument(
        "--skip-compile", action="store_true",
        help="Skip compilation and reuse a previously compiled binary "
             "in the work directory.",
    )
    p.add_argument(
        "--scheduler", default=None,
        help="Address of existing Dask scheduler (e.g. tcp://host:8786). "
             "If omitted, a LocalCluster is started.",
    )
    p.add_argument(
        "--n-workers", type=int, default=None,
        help="Number of workers for LocalCluster (default: number of CPUs). "
             "Ignored when --scheduler is set.",
    )
    p.add_argument(
        "--work-dir", default="./work",
        help="Directory for per-job filelist files and compiled binary "
             "(default: ./work)",
    )
    p.add_argument(
        "--results-file", default="results.json",
        help="Path to write the results summary (default: results.json)",
    )
    return p.parse_args()


def main() -> None:
    args = parse_args()

    # ── Load manifest ────────────────────────────────────────────────
    manifest_path = Path(args.manifest).resolve()
    with open(manifest_path) as f:
        manifest = expand_env_vars(json.load(f))

    output_dir = str(manifest_path.parent / manifest.get("output_dir", "./output"))
    output_pattern = manifest.get("output_pattern", "output_{job_id}.root")
    timeout = manifest.get("timeout_seconds", 3600)

    work_dir = Path(args.work_dir)
    work_dir.mkdir(parents=True, exist_ok=True)

    # ── Read filelist ────────────────────────────────────────────────
    filelist_path = Path(args.filelist).resolve()
    with open(filelist_path) as f:
        all_files = [line.strip() for line in f if line.strip()]

    if not all_files:
        print("ERROR: filelist is empty", file=sys.stderr)
        sys.exit(1)

    # ── Batch files into jobs ────────────────────────────────────────
    n = args.files_per_job
    batches = [all_files[i:i + n] for i in range(0, len(all_files), n)]

    jobs = []
    for idx, batch in enumerate(batches):
        job_id = f"job_{idx:04d}"
        # {first_filestem} = stem of first input file up to (not including) ".root"
        first_file = Path(batch[0])
        first_filestem = first_file.name
        while first_filestem.endswith(".root"):
            first_filestem = first_filestem[: -len(".root")]
        output_file = str(Path(output_dir) / output_pattern.format(
            job_id=job_id, first_filestem=first_filestem,
        ))
        jobs.append({
            "id": job_id,
            "files": batch,
            "output_file": output_file,
        })

    # ── Resolve binary (compile or pre-compiled) ─────────────────────
    if "source" in manifest:
        source_path = (manifest_path.parent / manifest["source"]).resolve()
        binary_name = source_path.stem
        binary_path = (work_dir / binary_name).resolve()

        if args.skip_compile:
            if not binary_path.exists():
                print(f"ERROR: --skip-compile but binary not found: {binary_path}",
                      file=sys.stderr)
                sys.exit(1)
            print(f"Skipping compilation, reusing: {binary_path}")
        else:
            print(f"Source: {source_path}")
            compile_source(
                source=source_path,
                output_binary=binary_path,
                include_dirs=manifest.get("include_dirs"),
                libraries=manifest.get("libraries"),
                compile_flags=manifest.get("compile_flags"),
            )
        binary = str(binary_path)
    elif "binary" in manifest:
        binary = manifest["binary"]
        print(f"Using pre-compiled binary: {binary}")
    else:
        print("ERROR: manifest must contain either 'source' or 'binary'",
              file=sys.stderr)
        sys.exit(1)

    print(f"\n{len(all_files)} input files → {len(jobs)} jobs "
          f"({args.files_per_job} files/job)")
    print(f"Binary:     {binary}")
    print(f"Output dir: {output_dir}")
    print(f"Work dir:   {args.work_dir}")
    print()

    # ── Cluster setup ────────────────────────────────────────────────
    if args.scheduler:
        print(f"Connecting to scheduler at {args.scheduler}")
        client = Client(args.scheduler)
    else:
        n_workers = args.n_workers or os.cpu_count()
        print(f"Starting LocalCluster with {n_workers} workers")
        cluster = LocalCluster(n_workers=n_workers, threads_per_worker=1)
        client = Client(cluster)

    print(f"Dashboard: {client.dashboard_link}\n")

    # ── Submit jobs ──────────────────────────────────────────────────
    # Capture the current environment so workers run the binary with
    # the same LD_LIBRARY_PATH, PATH, etc. as the submitting shell.
    run_env = dict(os.environ)

    futures = {}
    for job in jobs:
        fut = client.submit(
            run_cpp_job,
            job=job,
            binary=binary,
            work_dir=args.work_dir,
            timeout=timeout,
            env=run_env,
            key=f"job-{job['id']}",
        )
        futures[fut] = job["id"]

    # ── Collect results ──────────────────────────────────────────────
    results = []
    n_success = 0
    n_fail = 0

    for future in as_completed(futures):
        job_id = futures[future]
        try:
            result = future.result()
        except Exception as exc:
            result = {
                "job_id": job_id,
                "success": False,
                "returncode": None,
                "stdout": "",
                "stderr": f"Dask error: {exc}",
                "elapsed_seconds": 0,
            }

        if result["success"]:
            n_success += 1
            status = "OK"
        else:
            n_fail += 1
            status = f"FAIL (rc={result['returncode']})"

        print(f"  [{n_success + n_fail}/{len(jobs)}] {job_id}: {status} "
              f"({result['elapsed_seconds']}s)")
        results.append(result)

    # ── Write summary ────────────────────────────────────────────────
    results_path = Path(args.results_file)
    with open(results_path, "w") as f:
        json.dump(results, f, indent=2)

    print(f"\nDone: {n_success} succeeded, {n_fail} failed")
    print(f"Results written to {results_path}")

    client.close()


if __name__ == "__main__":
    main()
