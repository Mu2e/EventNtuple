# roodask — Dask Distributed C++ Job Runner

Compile and run a C++ ROOT macro across Dask workers.  Input files are
read from a filelist and automatically batched into jobs.

## Quick Start

```bash
# Create a filelist (one ROOT file per line)
ls /data/files/*.root > filelist.txt

# Edit jobs.json: set your source file, includes, and output pattern
vim jobs.json

# Compile and run locally — files distributed evenly across all CPUs
python roodask.py --manifest jobs.json --filelist filelist.txt

# 5 files per job, 4 workers
python roodask.py --manifest jobs.json --filelist filelist.txt \
    --files-per-job 5 --n-workers 4

# Only process the first 10 files (useful for testing)
python roodask.py --manifest jobs.json --filelist filelist.txt --max-files 10

# Skip recompilation on subsequent runs
python roodask.py --manifest jobs.json --filelist filelist.txt --skip-compile

# Merge all output files with hadd after completion
python roodask.py --manifest jobs.json --filelist filelist.txt \
    --hadd merged.root --hadd-j 4

# Connect to an existing Dask scheduler
python roodask.py --manifest jobs.json --filelist filelist.txt \
    --scheduler tcp://scheduler-host:8786
```

## File Overview

| File             | Description                                          |
|------------------|------------------------------------------------------|
| `roodask.py`    | Main script — compiles, batches, submits, collects   |
| `jobs.json`      | Manifest — source file, includes, output pattern     |
| `filelist.txt`   | Input filelist (one ROOT file per line)              |

## Manifest (`jobs.json`)

```json
{
  "source": "PlotEntranceMomentum.C",
  "include_dirs": ["${ROOT_INCLUDE_PATH}"],
  "libraries": [],
  "compile_flags": [],
  "output_dir": "./output",
  "output_pattern": "{first_filestem}_{job_id}.hist.root",
  "timeout_seconds": 3600
}
```

**Fields:**
- `source` — path to the C++ ROOT macro (relative to manifest).
  The macro's entry-point function must match the filename stem
  (e.g. `PlotEntranceMomentum.C` → `void PlotEntranceMomentum(...)`).
- `include_dirs` — *(optional)* extra `-I` include paths for compilation.
  Colon-separated values (like `${ROOT_INCLUDE_PATH}`) are automatically
  split into individual `-I` flags.
- `libraries` — *(optional)* extra `-l` libraries to link
- `compile_flags` — *(optional)* additional compiler flags (e.g. `["-O2"]`)
- `binary` — *(alternative to `source`)* path to a pre-compiled executable;
  skips compilation entirely
- `output_dir` — directory for output files (relative paths resolve from cwd)
- `output_pattern` — output filename pattern; available placeholders:
  - `{job_id}` — auto-generated job ID (e.g. `job_0000`)
  - `{first_filestem}` — filename of the first input file in the batch,
    stripped of the `.root` extension (e.g. `nts.ntuple.mock001`)
- `timeout_seconds` — per-job timeout (default: 3600)

**Environment variables:** All string values support `${VAR}` syntax,
expanded from the current shell environment at load time.

## Filelist

A plain text file with one input file path per line:

```
/data/run001/file_001.root
/data/run001/file_002.root
/data/run002/file_003.root
```

Blank lines are ignored.  Use `--max-files N` to process only the first
N files (useful for quick testing).

## Compilation

When `source` is specified, the script:

1. **Auto-generates a `main()` wrapper** (`work/<MacroName>_main.cpp`) that
   `#include`s the macro by filename (e.g. `#include "MyMacro.C"`) and
   calls its entry-point function with `argv[1]` (input filelist) and
   `argv[2]` (output file).  The macro's directory is added as an `-I`
   include path so the `#include` resolves correctly.
2. **Compiles** the wrapper with `g++` (or `$CXX` if set) and
   `root-config --cflags --libs`.

Additional compilation details:
- The macro's own directory is automatically added as an `-I` include path.
- `LD_LIBRARY_PATH` entries are added as `-L` and `-Wl,-rpath` flags so the
  correct `libstdc++` and other shared libraries are found at link time.
- `-Wl,--enable-new-dtags` converts RPATH to RUNPATH so `LD_LIBRARY_PATH`
  takes priority at runtime (works around GCC embedding its own lib64 as
  DT_RPATH).
- `-ltbb` is added automatically (required by ROOT's libImt but not
  included by `root-config`).
- Compilation is **incremental**: the script compares the source file's
  modification time against the binary and only recompiles when the source
  is newer.
- Use `--skip-compile` to force skipping compilation regardless of timestamps.
- Use `--force-compile` to force recompilation even if the binary is up to date.
- The binary is placed in the work directory (`./work/` by default).

## File Batching

Input files from the filelist are automatically split into jobs:

- **`--files-per-job N`** — each job gets exactly N files.
- **If omitted** — files are distributed evenly across the total number of
  worker slots (`n_workers × threads_per_worker`), so each slot gets
  roughly the same workload.

## Environment Propagation

The full environment (`PATH`, `LD_LIBRARY_PATH`, etc.) from the submitting
shell is captured and passed to each worker subprocess, ensuring the compiled
binary runs with the same library paths as the compilation environment.

## Merging with hadd

Use `--hadd <output.root>` to merge all successful job outputs into a single
ROOT file after all jobs complete.  If a relative path is given, the merged
file is placed in the output directory.  The individual per-job output files
are automatically deleted after a successful merge to save disk space.

Use `--hadd-j N` to control hadd parallelism (default: 1, set to 0 for all
available cores).

## Error Detection

Jobs are marked as **failed** if:
- The binary exits with a non-zero return code, OR
- The binary's stderr contains the word `Error` (catches ROOT errors like
  `TFile::TFile: file does not exist` that don't set a non-zero exit code)

## How It Works

1. Reads the manifest and filelist
2. Optionally truncates to `--max-files` files
3. Splits input files into batches (by `--files-per-job` or evenly across workers)
4. **Compiles** the C++ macro if the source is newer than the binary
   (unless `--skip-compile`)
5. Starts a `LocalCluster` or connects to an existing scheduler
6. Captures the current shell environment for worker subprocesses
7. For each batch/job, submits a task to a Dask worker that:
   - Writes a per-job filelist (`work/<job_id>_filelist.txt`)
   - Runs `<binary> <filelist> <output_file>`
   - Captures stdout, stderr, return code, and elapsed time
8. Collects results as jobs complete and prints progress
9. Writes `results.json` with all job outcomes (all paths are absolute)
10. Optionally merges outputs with `hadd` and cleans up individual files

## CLI Options

```
usage: roodask.py [-h] --manifest MANIFEST --filelist FILELIST
                   [--files-per-job N] [--max-files N]
                   [--hadd OUTPUT] [--hadd-j N]
                   [--skip-compile] [--force-compile]
                   [--scheduler SCHEDULER] [--n-workers N_WORKERS]
                   [--threads-per-worker N] [--work-dir WORK_DIR]
                   [--results-file RESULTS_FILE]

  --manifest             Path to jobs.json (required)
  --filelist             Path to text file with one input file per line (required)
  --files-per-job        Files per job (default: auto-distribute across workers)
  --max-files            Only process the first N files (default: all)
  --hadd                 Merge output ROOT files into this file after completion;
                         relative paths resolve into the output directory
  --hadd-j               Number of cores for hadd parallelism (default: 1)
  --skip-compile         Skip compilation, reuse binary in work directory
  --force-compile        Force recompilation even if the binary is up to date
  --scheduler            Dask scheduler address (e.g. tcp://host:8786)
  --n-workers            Workers for LocalCluster (default: all CPUs)
  --threads-per-worker   Threads per Dask worker (default: 1)
  --work-dir             Directory for filelists and compiled binary (default: ./work,
                         relative paths resolve from cwd)
  --results-file         Output results path (default: results.json)
```

## Results

After completion, `results.json` contains an array of result objects:

```json
{
  "job_id": "job_0000",
  "success": true,
  "returncode": 0,
  "stdout": "...",
  "stderr": "...",
  "elapsed_seconds": 12.34,
  "n_files": 5,
  "filelist": "/path/to/work/job_0000_filelist.txt",
  "output_file": "/path/to/output/nts.ntuple.mock001_job_0000.hist.root"
}
```
