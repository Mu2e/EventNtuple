# Copilot Instructions — roodask (Dask Distributed C++ Job Runner)

## Project Overview

This is a Python + C++ project for distributing ROOT macro execution across
Dask workers on a shared filesystem (Mu2e experiment at Fermilab).

**Main script:** `roodask.py` — a single-file Python CLI tool that:
1. Reads a JSON manifest (`jobs.json`) defining the C++ source, include paths,
   libraries, and output pattern.
2. Reads a filelist (text file, one ROOT file per line).
3. Auto-generates a `main()` C++ wrapper, compiles the ROOT macro into a
   standalone binary using `g++` + `root-config`.
4. Distributes input files across Dask workers (local or remote cluster).
5. Each worker runs the compiled binary via `subprocess`, passing a per-job
   filelist and output path.
6. Collects results (stdout, stderr, returncode, timing) into `results.json`.
7. Optionally merges output ROOT files with `hadd`.
8. Optionally runs a post-hadd command on the merged file (`--post-hadd`).

## Environment

- **OS:** Scientific Linux / RHEL on Fermilab machines
- **Filesystem:** Shared NFS (`/exp/mu2e/app/...`) — all workers see the same files
- **Python:** 3.12+ from `/cvmfs/mu2e.opensciencegrid.org/env/ana/current/`
- **Key packages:** `dask`, `distributed` (2026.1.x), `uproot` (available but not used here)
- **ROOT:** Available via Mu2e environment setup (provides `root-config`, `hadd`, etc.)
- **Compiler:** `g++` from PATH or `$CXX` env var; ROOT include/lib paths from `root-config`
- **Important env vars:** `ROOT_INCLUDE_PATH` (colon-separated), `LD_LIBRARY_PATH`

## Architecture Details

### Compilation (`compile_source()`)
- Generates `work/<MacroName>_main.cpp` that `#include`s the macro using the
  exact name from the manifest `"source"` field and calls its entry function
  (name = file stem) with `argv[1]` (filelist) and `argv[2]` (output).
  The macro must be findable via `include_dirs`.
- `source` path resolves from **cwd** (not manifest directory).
- Uses `$CXX` or `g++`, prints compiler version before building.
- `root-config --cflags --libs` provides ROOT flags.
- `LD_LIBRARY_PATH` entries are added as `-L` and `-Wl,-rpath` flags.
- `-Wl,--enable-new-dtags` ensures RUNPATH (not RPATH) so `LD_LIBRARY_PATH` wins at runtime.
- `-ltbb` is added (required by ROOT's libImt, not in `root-config --libs`).
- `include_dirs` values are split on `:` (os.pathsep) for colon-separated env vars.
- **Incremental:** only recompiles when source mtime > binary mtime.
- **Force recompile:** `--force-compile` skips the mtime check and always recompiles.
  Prints a distinct message ("Forcing recompilation (--force-compile)") vs the
  timestamp-triggered message ("Source is newer than binary, recompiling...").

### Path Resolution
- `source` from manifest: relative paths resolve from **cwd** (not manifest directory).
- `output_dir` from manifest: relative paths resolve from **cwd** (not manifest directory).
- `--work-dir`: relative paths resolve from **cwd**.
- `--hadd` target: relative paths resolve into the output directory.
- All printed paths and paths in `results.json` are absolute.

### Job Execution (`run_cpp_job()`)
- Each job receives a batch of input files, writes them to `work/<job_id>_filelist.txt`.
- If manifest has `"args"`, builds command as `<binary> [args...]` with `{filelist}` and
  `{output}` placeholders substituted per job.
- If no `"args"`, defaults to: `<binary> <filelist_path> <output_file>`
- The submitting shell's full environment is captured and passed to workers via `env=`.
- Success = returncode 0 AND no "Error" in stderr (catches ROOT silent failures).

### Manifest (`jobs.json`)
- All string values undergo `${VAR}` expansion via `os.path.expandvars()` at load time.
- `output_pattern` supports `{job_id}` and `{first_filestem}` placeholders.
- `"args"` field: list of strings with `{filelist}` and `{output}` placeholders,
  allowing the user to control the full command-line argument order.
- `config_template` field was removed — the script generates the wrapper automatically.

### File Batching
- If `--files-per-job` is given, uses that exact batch size.
- If omitted, auto-distributes: `ceil(n_files / (n_workers * threads_per_worker))`.

## Key Design Decisions

1. **Single file script** — `roodask.py` is intentionally one file for easy copying/sharing.
2. **No config template needed** — the main() wrapper is auto-generated, not templated.
3. **Binary invocation:** Default is `<binary> <filelist> <output>`. If `"args"` is set
   in the manifest, the user controls the full argument list with `{filelist}` and
   `{output}` placeholders (e.g. `["--input", "{filelist}", "--out", "{output}", "flag"]`).
4. **Shared filesystem assumed** — workers read/write the same paths as the submitter.
5. **Environment propagation** — `os.environ` is serialized and passed to each worker
   subprocess to ensure consistent library resolution.

## Common Tasks

- **Adding a new CLI flag:** Edit `parse_args()`, then use `args.<flag>` in `main()`.
- **Changing the binary interface:** Edit the wrapper template in `compile_source()`
  (the `wrapper_path.write_text(...)` block) and update `run_cpp_job()` subprocess call.
- **Adding post-processing:** Add after the post-hadd section in `main()`, before `client.close()`.
- **Supporting a new manifest field:** Add to `jobs.json`, access via `manifest.get("field")`.

## Files

| File                    | Purpose                                              |
|-------------------------|------------------------------------------------------|
| `roodask.py`           | Main script (compilation, batching, execution, merge)|
| `jobs.json`             | Manifest (source, includes, output pattern)          |
| `filelist.txt`          | Input file paths (one per line)                      |
| `PlotEntranceMomentum.C`| Example ROOT macro                                  |
| `README.md`             | User-facing documentation                            |
| `work/`                 | Generated: compiled binary, wrapper, per-job filelists|
| `output/`               | Generated: per-job output ROOT files                 |
| `results.json`          | Generated: job outcomes (success, timing, stderr)    |
