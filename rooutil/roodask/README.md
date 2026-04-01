# Dask Distributed C++ Job Runner

Run a C++ program across Dask workers with per-job config files.

## Quick Start

```bash
# Edit jobs.json: set your binary path, input files, and parameters
vim jobs.json

# Edit config_template.txt to match your program's config format
vim config_template.txt

# Run locally (uses all CPUs)
python run_jobs.py --manifest jobs.json

# Run with 4 local workers
python run_jobs.py --manifest jobs.json --n-workers 4

# Connect to an existing Dask scheduler
python run_jobs.py --manifest jobs.json --scheduler tcp://scheduler-host:8786
```

## File Overview

| File                 | Description                                      |
|----------------------|--------------------------------------------------|
| `run_jobs.py`        | Main script — submits and collects C++ jobs      |
| `jobs.json`          | Job manifest — defines binary, files, parameters |
| `config_template.txt`| Config template with `{placeholder}` fields      |

## Job Manifest (`jobs.json`)

```json
{
  "binary": "/path/to/my_cpp_program",
  "config_template": "config_template.txt",
  "output_dir": "./output",
  "timeout_seconds": 3600,
  "jobs": [
    {
      "id": "job_001",
      "input_file": "/path/to/data/file_001.root",
      "params": {
        "n_events": 10000,
        "cut_value": 0.5
      }
    }
  ]
}
```

**Fields:**
- `binary` — path to the compiled C++ executable
- `config_template` — path to the config template (relative to manifest)
- `output_dir` — directory for program output (relative to manifest)
- `timeout_seconds` — per-job timeout (default: 3600)
- `jobs` — array of job definitions:
  - `id` — unique job identifier
  - `input_file` — input file path
  - `params` — dict of parameters (keys must match template placeholders)

## Config Template

The template uses Python `{placeholder}` syntax. Available built-in variables:

- `{input_file}` — from the job's `input_file` field
- `{output_file}` — auto-generated as `<output_dir>/<job_id>.out`

Any keys in `params` are also available. Example:

```
input_file  = {input_file}
output_file = {output_file}
n_events    = {n_events}
cut_value   = {cut_value}
```

## CLI Options

```
usage: run_jobs.py [-h] --manifest MANIFEST [--scheduler SCHEDULER]
                   [--n-workers N_WORKERS] [--work-dir WORK_DIR]
                   [--results-file RESULTS_FILE]

  --manifest       Path to jobs.json (required)
  --scheduler      Dask scheduler address (e.g. tcp://host:8786)
  --n-workers      Workers for LocalCluster (default: all CPUs)
  --work-dir       Directory for per-job configs (default: ./work)
  --results-file   Output results path (default: results.json)
```

## Results

After completion, `results.json` contains an array of result objects:

```json
{
  "job_id": "job_001",
  "success": true,
  "returncode": 0,
  "stdout": "...",
  "stderr": "...",
  "elapsed_seconds": 12.34,
  "config_path": "./work/job_001.cfg",
  "output_file": "./output/job_001.out"
}
```

## How It Works

1. Reads the job manifest and config template
2. Starts a `LocalCluster` or connects to an existing scheduler
3. For each job, submits a task to a Dask worker that:
   - Renders the config template with job-specific values
   - Writes the config to `<work_dir>/<job_id>.cfg`
   - Runs the C++ binary via `subprocess.run` with the config path as argument
   - Captures stdout, stderr, return code, and elapsed time
4. Collects results as jobs complete and prints progress
5. Writes `results.json` with all job outcomes
