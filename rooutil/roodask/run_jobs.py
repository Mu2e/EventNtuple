#!/usr/bin/env python3
"""
run_jobs.py — Dask Distributed C++ Job Runner

Submits C++ program invocations across Dask workers.  Each job gets a
per-job config file rendered from a template, and stdout/stderr/return
code are collected into a results summary.

Usage:
    # Local cluster (uses all CPUs by default)
    python run_jobs.py --manifest jobs.json

    # Local cluster with 4 workers
    python run_jobs.py --manifest jobs.json --n-workers 4

    # Connect to an existing scheduler
    python run_jobs.py --manifest jobs.json --scheduler tcp://scheduler-host:8786
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import time
from pathlib import Path

from dask.distributed import Client, LocalCluster, as_completed


# ── Task function (runs on each worker) ─────────────────────────────

def run_cpp_job(
    job: dict,
    binary: str,
    config_template: str,
    work_dir: str,
    output_dir: str,
    timeout: int,
) -> dict:
    """Run a single C++ job: render config, execute binary, capture output."""

    job_id = job["id"]
    input_file = job["input_file"]
    params = job.get("params", {})

    # Ensure per-job directories exist
    work_path = Path(work_dir)
    work_path.mkdir(parents=True, exist_ok=True)
    out_path = Path(output_dir)
    out_path.mkdir(parents=True, exist_ok=True)

    output_file = str(out_path / f"{job_id}.out")

    # Build the replacement map from job fields + params
    replacements = {
        "input_file": input_file,
        "output_file": output_file,
        **params,
    }

    # Render the config
    config_text = config_template.format_map(replacements)
    config_path = work_path / f"{job_id}.cfg"
    config_path.write_text(config_text)

    # Run the C++ binary
    t0 = time.monotonic()
    try:
        result = subprocess.run(
            [binary, str(config_path)],
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        elapsed = time.monotonic() - t0
        return {
            "job_id": job_id,
            "success": result.returncode == 0,
            "returncode": result.returncode,
            "stdout": result.stdout,
            "stderr": result.stderr,
            "elapsed_seconds": round(elapsed, 2),
            "config_path": str(config_path),
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
            "config_path": str(config_path),
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
            "config_path": str(config_path),
            "output_file": output_file,
        }


# ── CLI & main ───────────────────────────────────────────────────────

def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description="Run a C++ program across Dask workers",
    )
    p.add_argument(
        "--manifest", required=True,
        help="Path to jobs.json manifest file",
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
        help="Directory for per-job config files (default: ./work)",
    )
    p.add_argument(
        "--results-file", default="results.json",
        help="Path to write the results summary (default: results.json)",
    )
    return p.parse_args()


def main() -> None:
    args = parse_args()

    # ── Load manifest ────────────────────────────────────────────────
    manifest_path = Path(args.manifest)
    with open(manifest_path) as f:
        manifest = json.load(f)

    binary = manifest["binary"]
    template_path = manifest_path.parent / manifest["config_template"]
    config_template = template_path.read_text()
    output_dir = str(manifest_path.parent / manifest.get("output_dir", "./output"))
    timeout = manifest.get("timeout_seconds", 3600)
    jobs = manifest["jobs"]

    print(f"Loaded {len(jobs)} jobs from {manifest_path}")
    print(f"Binary:          {binary}")
    print(f"Config template: {template_path}")
    print(f"Output dir:      {output_dir}")
    print(f"Work dir:        {args.work_dir}")
    print()

    # ── Cluster setup ────────────────────────────────────────────────
    if args.scheduler:
        print(f"Connecting to scheduler at {args.scheduler}")
        client = Client(args.scheduler)
    else:
        n = args.n_workers or os.cpu_count()
        print(f"Starting LocalCluster with {n} workers")
        cluster = LocalCluster(n_workers=n, threads_per_worker=1)
        client = Client(cluster)

    print(f"Dashboard: {client.dashboard_link}\n")

    # ── Submit jobs ──────────────────────────────────────────────────
    futures = {}
    for job in jobs:
        fut = client.submit(
            run_cpp_job,
            job=job,
            binary=binary,
            config_template=config_template,
            work_dir=args.work_dir,
            output_dir=output_dir,
            timeout=timeout,
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
