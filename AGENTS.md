# AGENTS.md — EventNtuple Coding Agent Instructions

## Project Overview

EventNtuple is a Mu2e (Fermilab) art/ROOT analysis ntuple package. It fills a flat
ROOT ntuple from Mu2e reconstruction output (KalSeeds, CaloCluster, CRV, etc.) and
provides a Python/ROOT analysis library (RooUtil) and distributed job runner (roodask).

**Framework:** `art` (Fermilab) + `cetmodules` CMake layer + `fhiclcpp` configuration.  
**Languages:** C++17 (modules, helpers, structs), FHiCL (job config), Python 3.12+ (helpers/tools).

---

## Build Commands

Environment must be set up via Muse before building:

```bash
mu2einit
muse setup          # or: muse setup AnalysisMDC2020 / AnalysisMDC2025
muse build -j4 --mu2eCompactPrint
```

Alternative Spack-based build:

```bash
spack develop event-ntuple@main
spack concretize -f && spack install
```

Run the ntuple maker:

```bash
mu2e -c EventNtuple/fcl/from_mcs-mockdata.fcl -S your-art-filelist.txt
```

---

## Test / Validation Commands

All validation scripts are in `validation/`. There is no unit test framework; tests
run the full art job and compare histograms.

```bash
# Quick smoke test: runs from_mcs-mockdata.fcl, creates histograms
bash validation/quick_test.sh

# Test all FCL files for a campaign
bash validation/test_fcls.sh MDC2020
bash validation/test_fcls.sh MDC2025
bash validation/test_fcls.sh Run1B

# Test RooUtil library
bash validation/test_rooutil.sh

# Test all RooUtil example macros
bash validation/test_rooutil_examples.sh

# Test Python ntuplehelper
python validation/ntuplehelper-test.py

# Test roodask distributed runner
bash validation/roodask_test.sh
```

To run a **single FCL test** manually:

```bash
mu2e -c EventNtuple/fcl/<specific>.fcl -S filelist.txt -n 100
```

---

## FHiCL Configuration Structure

### Branch / MC Data Configuration (Key Area for Improvement)

Branch configuration is defined in `fcl/prolog.fcl`. Each track type is a FHiCL table:

```
DeM : {
  input   : "MergeKKDeM"        # input KalSeedPtr collection tag
  branch  : "dem"               # output ROOT branch name
  options : {
    fillMC         : true       # toggle MC truth filling for this branch
    fillHits       : true       # toggle hit-level info
    genealogyDepth : -1         # MC genealogy depth (-1 = all)
    matchDepth     : -1         # MC match depth (-1 = all)
  }
  trkQualTags : ["TrkQualDeM"]
  trkPIDTags  : ["TrkPIDDeM"]
}
```

Global MC switch lives in `EventNtupleMaker` config: `FillMCInfo : true/false`.  
Per-branch MC is controlled by `options.fillMC`. Both must be true for MC to fill.

**Current issue:** Branch on/off and MC on/off are scattered across individual track
tables in `prolog.fcl` and duplicated in every campaign FCL. When adding a new
track type or toggling MC for a subset of branches, edits are required in many places.

**Preferred direction for improvement:**
- Define a single `BranchOptions` prolog table with canonical defaults.
- Use `@table::` inheritance for per-branch overrides rather than full re-specification.
- Consider a top-level `MCOptions` table that can be overlaid to flip all `fillMC` flags.
- Campaign FCLs should `#include prolog.fcl` and only override what differs.

---

## C++ Code Style

### File Naming
- Headers: `PascalCase.hh` in `inc/`
- Sources: `PascalCase.cc` in `src/`
- Module plugins: `PascalCase_module.cc`

### Namespaces
All production code lives in `namespace mu2e {}`.  
RooUtil library uses `namespace rooutil {}`.

### Struct / Class Naming
- Ntuple info structs: `PascalCase` ending in `Info` or `InfoMC` (e.g. `TrkInfo`, `CaloClusterInfoMC`)
- Each struct lives in its own `inc/<StructName>.hh` header

### Member Naming
- Module private members: leading underscore (`_conf`, `_fillmc`, `_ntuple`)
- Struct data members: `snake_case` (no underscore suffix); initialized to sentinel values
  - Integers: `int status = -1;`
  - Floats: `float chisq = -1;` or `float mom = std::numeric_limits<float>::min();`
  - Booleans: `bool flag = false;`

### Info Struct Pattern (required for `ntuplehelper` compatibility)
```cpp
namespace mu2e {
  struct FooInfo {
    // required comment on every leaf for ntuplehelper autodoc
    int    status = -1;   // fit status
    float  mom    = -1;   // momentum at tracker entrance (MeV/c)

    void reset() { *this = FooInfo(); }
  };
}
```
Every leaf **must** have an inline `//` comment — this is parsed by `ntuplehelper --list-all-branches`.

### Includes (C++ ordering)
1. Mu2e Offline headers (`Offline/…`)
2. art framework headers (`art/…`, `canvas/…`, `fhiclcpp/…`)
3. ROOT headers
4. Local EventNtuple headers (`EventNtuple/inc/…`)
5. C++ standard library (`<vector>`, `<string>`, etc.)

### Error Handling
- Throw via `cet::exception`: `throw cet::exception("EventNtuple") << "message";`
- Do not use raw `std::exception` or `exit()` in art modules
- Warnings via `mf::LogWarning("EventNtuple") << "message";`

### FHiCL Config Structs (in modules)
```cpp
struct Config {
  fhicl::Atom<bool>     fillMC  { Name("FillMCInfo"),  Comment("Fill MC info"), true };
  fhicl::Table<SubConf> sub     { Name("SubConfig") };
  fhicl::Sequence<art::InputTag> tags { Name("InputTags") };
};
```

---

## Python Code Style

- Python 3.12+; no type annotations required but welcome
- `snake_case` for all functions, methods, and variables
- Class names: `PascalCase`
- No f-strings required; format strings acceptable
- `roodask/roodask.py` is intentionally a **single-file script** — do not split it

---

## FHiCL Style Guidelines

- Use `BEGIN_PROLOG` / `END_PROLOG` for all reusable tables
- Use `@local::` for prolog references; `@table::` for struct inheritance/merging
- Do not duplicate full table bodies — inherit with `@table::Base` then override fields
- One `#include` per dependency; include Offline prologs before EventNtuple prologs
- Comment all numeric constants with units: `MaxDE : 500.0 # MeV`
- Boolean flags: `true` / `false` (lowercase, no quotes)

---

## Adding a New Branch (Developer Checklist)

See `doc/developers.md` for the full walkthrough. Summary:

1. Create `inc/FooInfo.hh` with commented struct (see struct pattern above)
2. Add fill logic to `src/InfoStructHelper.cc` (or `InfoMCStructHelper.cc` for MC)
3. Register branch in `src/EventNtupleMaker_module.cc`
4. Add prolog table to `fcl/prolog.fcl` using `@table::` inheritance
5. Regenerate `doc/branches.md`: `ntuplehelper --list-all-branches --export-to-md`
6. Run `bash validation/quick_test.sh` to confirm no runtime errors

---

## Key Files Reference

| File | Role |
|---|---|
| `src/EventNtupleMaker_module.cc` | Main `art::EDAnalyzer`; branch registration and filling |
| `src/InfoStructHelper.cc` | Reco → struct fill logic |
| `src/InfoMCStructHelper.cc` | MC truth → struct fill logic |
| `inc/*Info*.hh` | One ntuple branch struct per file |
| `fcl/prolog.fcl` | Master FHiCL prolog (track types, branch configs, MC options) |
| `fcl/from_mcs-mockdata.fcl` | Default/reference job FCL |
| `helper/ntuplehelper.py` | Python ntuple inspection tool |
| `rooutil/inc/RooUtil.hh` | ROOT analysis library |
| `rooutil/roodask/roodask.py` | Dask distributed job runner (single-file) |
| `validation/quick_test.sh` | Primary smoke test |
| `doc/developers.md` | Developer guide for adding branches |

---

## roodask Sub-tool (rooutil/roodask/)

See `rooutil/roodask/.github/copilot-instructions.md` for detailed architecture.  
Key points: single-file CLI, auto-generates C++ `main()` wrapper, shared NFS filesystem,
full environment propagated to Dask workers. Do not split `roodask.py` into multiple files.
