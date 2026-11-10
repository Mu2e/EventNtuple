# FCL inventory

First step for [#349](https://github.com/Mu2e/EventNtuple/issues/349): what each FCL in `fcl/` is for, who
wrote and maintains it, and what state it is in, so that consolidation can be planned against real use cases.

Snapshot of `main` at `2251849` (2026-11-09). Scope is this repository only: use of these FCLs by other
repositories or by production was not surveyed, so check before renaming or removing any of them.

## Summary

- **33 files**: 2 prologs (`prolog.fcl`, `prolog_mc_reweighters.fcl`) and 31 job FCLs.
- **One base for almost everything**: 21 of the 31 jobs `#include` `from_mcs-mockdata.fcl`, directly or through
  another job (up to three levels deep). The other 10 build their own `physics` block from `prolog.fcl`.
- **Status**: 29 jobs parse with `fhicl-dump`, and 2 do not (`from_mcs-reflection.fcl`, `TrkAnaLineFromDigis.fcl`).
  Parsing is only a configuration check: these jobs were not run.
- **Validation**: 17 jobs are run by `validation/commands.txt`. The other 14 are not tested anywhere in the repo.
- **Documentation**: the table in `fcl/README.md` lists 19 files: two of those names are wrong and 12 files
  are missing.

## Use cases

Users are taken from the git history: the people who created a file and who change it most often.

| Use case | Job FCLs | Main authors |
|---|---|---|
| Standard ntuple of reconstructed field-on MC (`mcs`) | `from_mcs-mockdata`, `-mockdata_noMC`, `-primary`, `-mixed`, `-OffSpill` | Andrew Edmonds |
| Cosmics in the extracted position, and KPP data | `from_mcs-extracted`, `-extracted_withCrvPulses`, `from_rec-combined`, `from_rec-crv-kpp`, `-crv-kpp_withCrvDigis`, `-crv-kpp-MC` | Andrew Edmonds, Yuri Oksuzian, Rob Mina, David Brown, Michael MacKenzie |
| Run-1B (straight-line fits, time clusters, line seeds) | `from_mcs-Run1B` | Andrew Edmonds, Michael MacKenzie, Iuri Oksuzian |
| Reconstruction and ntuple in one job, from digis | `from_dig-mockdata`, `from_dig-calo`, `from_dig-OnSpill` (trigger tracks), `TrkAnaLineFromDigis` | Andrew Edmonds, David Brown, Sophie Middleton |
| Tracker calibration: the best De track per event, with hits | `from_mcs-DeCalib`, `from_dig-DeCalib`, `from_dig-DeCalib_seed`, `CompareDeTracks` | David Brown |
| Reflecting cosmics (upstream and downstream fits of one particle) | `from_mcs-RefSelect`, `-RefExtrapSelecte`, `-RefExtrapSelectmu`, `-reflection` | David Brown |
| Validation of EventNtuple itself | `from_mcs-ceSimReco`, `-ceSimRecoVal` | Andrew Edmonds |
| Examples of one option | `from_mcs-mockdata_separateTrkBranches`, `-mockdata_selectorExample`, `-mixed_trkQualCompare`, `-primary_addVDSteps`, `-mockdata_crvInf` | Andrew Edmonds, Samuel Grant |

## Include tree

```
prolog.fcl  (includes prolog_mc_reweighters.fcl)
├── from_mcs-mockdata.fcl
│   ├── from_mcs-mockdata_noMC.fcl
│   │   ├── from_rec-crv-kpp.fcl
│   │   │   └── from_rec-crv-kpp_withCrvDigis.fcl
│   │   └── from_rec-combined.fcl
│   ├── from_mcs-primary.fcl
│   │   ├── from_mcs-mixed.fcl
│   │   └── from_mcs-primary_addVDSteps.fcl
│   ├── from_mcs-extracted.fcl
│   │   ├── from_mcs-extracted_withCrvPulses.fcl
│   │   └── from_mcs-Run1B.fcl
│   ├── from_mcs-ceSimReco.fcl
│   │   └── from_mcs-ceSimRecoVal.fcl
│   ├── from_mcs-OffSpill.fcl
│   ├── from_mcs-DeCalib.fcl
│   ├── from_mcs-RefExtrapSelecte.fcl
│   ├── from_mcs-RefExtrapSelectmu.fcl
│   ├── from_rec-crv-kpp-MC.fcl
│   ├── from_mcs-mockdata_separateTrkBranches.fcl
│   ├── from_mcs-mockdata_selectorExample.fcl
│   ├── from_mcs-mixed_trkQualCompare.fcl
│   └── from_mcs-mockdata_crvInf.fcl
├── from_dig-mockdata.fcl         (+ Production recoMC prolog)
├── from_dig-calo.fcl
├── from_dig-OnSpill.fcl          (+ Production recoMC prolog)
├── from_dig-DeCalib.fcl          (+ Production recoMC prolog)
├── from_dig-DeCalib_seed.fcl     (+ Production recoMC prolog)
├── from_mcs-RefSelect.fcl        (+ Production/JobConfig/recoMC/RefExtrapSelect.fcl)
├── from_mcs-reflection.fcl
├── CompareDeTracks.fcl
└── TrkAnaLineFromDigis.fcl       (+ Production reco prolog)
```

## Per-file inventory

"Validated" means the job is run by `validation/commands.txt`. "Last change" is the latest commit on `main` that
touches the file.

### Built on `from_mcs-mockdata.fcl`

| File | Input | What it sets on top of its base | Authors (commits) | Last change | Validated |
|---|---|---|---|---|---|
| `from_mcs-mockdata.fcl` | `mcs`, field-on MC (mock/ensemble datasets) | The reference job: `PathOnline`; offline track fits plus three online (trigger) fit branches; offline and online helices; `genEventCount` off | Edmonds (4), Brown (2), MacKenzie (1), Middleton (1) | 2026-10-13 | yes |
| `from_mcs-mockdata_noMC.fcl` | `mcs`, with MC truth not wanted | `mc.fill` and `calo.mc.fill` off; `PathNoMCOnline` | Edmonds (5), MacKenzie (1) | 2026-10-13 | yes |
| `from_mcs-primary.fcl` | `mcs`, primary only (no background) | `genEventCount` back on | Edmonds (3) | 2026-07-27 | yes |
| `from_mcs-mixed.fcl` | `mcs`, primary plus background | Nothing: it only includes `from_mcs-primary.fcl` | Edmonds (2) | 2024-10-14 | yes |
| `from_mcs-primary_addVDSteps.fcl` | `mcs`, primary | Adds the `mcsteps_virtualdetector` branch | Edmonds (3) | 2026-06-04 | yes |
| `from_mcs-OffSpill.fcl` | `mcs`, off-spill cosmics, field on | `CentralHelix` fits (`Off`) plus online fits; `PathOffOnline` | Brown (1), MacKenzie (1) | 2026-10-13 | no |
| `from_mcs-extracted.fcl` | `mcs`, extracted-position cosmics | `KinematicLine` fits (`Ext`), `PathExt`, no TrkDtDt, `genEventCount` on, extracted geometry | Edmonds (10), Brown (1) | 2026-08-10 | yes |
| `from_mcs-extracted_withCrvPulses.fcl` | `mcs`, extracted all-events stream | Every CRV pulse, including unclustered ones; truth planes for T1, EX, T2 and M | Oksuzian (2) | 2026-10-29 | no |
| `from_mcs-Run1B.fcl` | `mcs`, Run-1B `-KL` datasets | Merges three line-fit collections; time clusters and line seeds (with hits for two of them); VD steps; standard geometry | Edmonds (10), MacKenzie (8), Oksuzian (2) | 2026-10-29 | yes |
| `from_mcs-ceSimReco.fcl` | Output of `Production/Validation/ceSimReco.fcl` | Only De fits, with De-specific PID producers; no trigger info | Edmonds (9), MacKenzie (2) | 2026-10-29 | yes |
| `from_mcs-ceSimRecoVal.fcl` | Output of `validation/ceTrig.fcl` | Adds `trkhitcalibs` and trigger info (`ceTrig` process) | Edmonds (4) | 2026-06-04 | yes |
| `from_mcs-DeCalib.fcl` | `mcs`, primary or mixed | Keeps events with a calibration-quality De and writes only the best De track (`ENDeCalib`); no subrun ntuple | Brown (3), Edmonds (3) | 2026-10-24 | yes |
| `from_mcs-RefExtrapSelecte.fcl` | `mcs`, output of the reflection extrapolation selection | Ntuple of `SelectReflecte` with its own TrkQual, TrkDtDt and TrkPID | Brown (1), Middleton (2), MacKenzie (1) | 2026-10-29 | no |
| `from_mcs-RefExtrapSelectmu.fcl` | As above | The same for `SelectReflectmu` | Brown (4), Middleton (2), MacKenzie (1) | 2026-11-06 | no |
| `from_rec-crv-kpp-MC.fcl` | Reconstructed KPP-geometry CRV MC | CRV only, with MC truth so the truth-plane branches fill; T1, EX and T2 planes; empty producer path; v02 extracted geometry | Mina (2) | 2026-06-18 | no |
| `from_mcs-mockdata_separateTrkBranches.fcl` | `mcs`, example | Separate `de`, `ue`, `dm` and `um` branches (`PathSeparate`) | Edmonds (5) | 2026-08-10 | yes |
| `from_mcs-mockdata_selectorExample.fcl` | `mcs`, example | A `SimpleKalSeedSelector` on `MergeKKAll` | Edmonds (2) | 2024-10-14 | yes |
| `from_mcs-mixed_trkQualCompare.fcl` | `mcs`, example | Three TrkQual versions side by side; needs ONNX files that are not in the release | Edmonds (7), MacKenzie (2) | 2026-10-29 | no |
| `from_mcs-mockdata_crvInf.fcl` | `mcs`, MDC2020 geometry | Adds the `CrvInference` producer and its branch | Grant (1), Edmonds (1), MacKenzie (1) | 2026-10-13 | no |

Built on `from_mcs-mockdata_noMC.fcl`:

| File | Input | What it sets on top of its base | Authors (commits) | Last change | Validated |
|---|---|---|---|---|---|
| `from_rec-crv-kpp.fcl` | CRV KPP data (`rec`) | CRV only: tracker and calorimeter off, plain CRV module labels, extracted geometry | Edmonds (3) | 2026-06-04 | yes |
| `from_rec-crv-kpp_withCrvDigis.fcl` | CRV KPP data | Adds `crvdigis` | Edmonds (2) | 2026-06-01 | yes |
| `from_rec-combined.fcl` | Combined cosmic pass-1 data (`rec`), moved here from the PassN repository | Line fits, EvB trigger info, time clusters, line seeds with hits, CRV digis, calorimeter, MSD hits | Brown (1), MacKenzie (2) | 2026-11-03 | no |

### Standalone jobs (their own `physics` block)

| File | Input | What it does | Authors (commits) | Last change | Validated |
|---|---|---|---|---|---|
| `from_dig-mockdata.fcl` | `dig`, field-on MC | Runs on-spill reconstruction, then the standard ntuple; also writes an `mcs` file; validation database | Edmonds (6), Brown (1) | 2026-07-27 | yes |
| `from_dig-calo.fcl` | `dig`, calorimeter studies | Calorimeter digis and digi truth only; tracker, trigger and CRV coincidences off | Middleton (1), Edmonds (3) | 2026-07-27 | yes |
| `from_dig-OnSpill.fcl` | `dig`, trigger output | One ntuple per trigger track algorithm (apr, tpr, cpr, mpr) with MC matching, from `EventNtupleMakerTTMC` | Edmonds (5), Brown (3) | 2026-08-12 | yes |
| `from_dig-DeCalib.fcl` | `dig`, primary or mixed | De reconstruction, then the DeCalib ntuple with TrkQual and TrkPID; validation database | Brown (9), MacKenzie (1) | 2026-10-29 | no |
| `from_dig-DeCalib_seed.fcl` | `dig`, primary or mixed | As above but with the KinKal seed fit and no TrkPID | Brown (3), MacKenzie (1) | 2026-10-29 | no |
| `CompareDeTracks.fcl` | `mcs` with two De collections | Pairs "the same" De track from two collections (`SelectSameTrack`); the secondary collection must be set by the user (`@nil`) | Brown (2), Edmonds (1), MacKenzie (1) | 2026-10-29 | no |
| `from_mcs-RefSelect.fcl` | Output of `Production/JobConfig/reco/RefT0Select.fcl` | Regrows and selects reflections using Production's `RefExtrapSelect.fcl`, then writes `ENRefe` and `ENRefmu` | Brown (3), Middleton (2), MacKenzie (1) | 2026-10-29 | no |
| `from_mcs-reflection.fcl` | `mcs` | Older reflection selection (`SelectReflections`) with two ntuples and a livetime printout | Edmonds (5), Brown (3), Middleton (1), MacKenzie (1) | 2026-10-29 | no |
| `TrkAnaLineFromDigis.fcl` | `dig`, extracted cosmics (MDC2020y) | Straight-line reconstruction with no field, then an ntuple of `KKLine` with failed fits kept | Edmonds (7), Brown (1), Bonventre (1), Middleton (1) | 2026-10-23 | no |

### Prologs

| File | Contents | Authors (commits) |
|---|---|---|
| `prolog.fcl` | TrkQual, TrkPID and trigger producers; `MergeKK*`; per-fit branch tables (`De`, `Ue`, `Dmu`, `Umu`, `Ext`, `Off`, `All`, `DeCalib`, `Trig*`); `EventNtupleMaker` defaults; the `EventNtuple` producers, analyzers and paths; `EventNtupleMakerTTMC`; `ENDeCalib` | 190 commits from 17 people; Edmonds (111), Brown (31), MacKenzie (18) |
| `prolog_mc_reweighters.fcl` | `DIOWeight`, `PBIWeight`, `RMCWeight`, `dioLLWeight` | Edmonds (1), Culbertson (1) |

## Problems found

### Broken or not doing what they say

1. **`from_mcs-reflection.fcl` does not parse.** It uses `@table::TrkPID`, which `prolog.fcl` now defines only
   as `TrkPIDProducers.TrkPID`. Two more problems would remain after that fix: its `ENBranch` table sets `branch`
   where the analyzer expects `branchname`, and its PID modules set `KalSeedPtrCollection` where the prolog's
   TrackPID configurations use `KalSeeds`. It looks superseded by `from_mcs-RefSelect.fcl` and the
   `RefExtrapSelect` pair, but that needs confirming.
2. **`TrkAnaLineFromDigis.fcl` does not parse.** `Tracking.producers` no longer exists in Offline or Production.
   The file still uses the old TrkAna names and is not run by any test.
3. **`from_mcs-mockdata_selectorExample.fcl` does not select the best track.** It sets `SelectBest : true` at
   the top level of the file, where nothing reads it. The parsed configuration has no `SelectBest` on
   `MergeKKAll`, so the example writes every selected track instead of one per event.
4. **Some jobs need a recent Production.** `from_mcs-RefSelect.fcl` (needs
   `Production/JobConfig/recoMC/RefExtrapSelect.fcl`) and the two `from_dig-DeCalib` jobs (need `RecoMC.MCReco`)
   fail to parse with the Production checkout of 2026-09-22 but parse with the current Production `main`.
   Nothing records which Production version is required.

### Duplication

- **Per-collection TrkQual, TrkDtDt and TrkPID**: the same three producer blocks, pointed at one KalSeed
  collection, are written out 8 times across 6 files (twice each in `RefSelect` and `reflection`, once each in
  both `RefExtrapSelect` files, `from_mcs-DeCalib` and `from_dig-DeCalib`). `TrkPIDDeCalib` is defined
  separately in the two DeCalib files.
- **`RefExtrapSelecte` and `RefExtrapSelectmu`** are the same 65 lines with `e` replaced by `mu`, and
  `from_mcs-RefSelect.fcl` contains both analyzers again.
- **`from_dig-DeCalib.fcl` and `from_dig-DeCalib_seed.fcl`** share about 45 lines; the seed version differs
  only in the fit and in dropping TrkPID.
- **The CRV-only settings** (tracker and calorimeter off, `RecoCountTag` and `PBTTag` empty, `hasCRV`,
  `FillTriggerInfo`) are repeated in `from_rec-crv-kpp.fcl` and `from_rec-crv-kpp-MC.fcl`, because one inherits
  from the no-MC base and the other from the MC base.
- **`from_mcs-mixed.fcl`** is identical to `from_mcs-primary.fcl`.
- **In `prolog.fcl`**, `MergeKKDe` is defined twice and listed twice in `MergeKKProducers`.
- **`from_rec-combined.fcl`** sets `crv.fill` to false and then to true, and sets `mc.fill : false`, which its
  base already does.

### Inheritance

- **Use-case choices are made in the shared base.** `from_mcs-mockdata.fcl` is both the base of 21 jobs and the
  standard field-on job, so its specific choices (online fits, helices, `PathOnline`, `genEventCount` off)
  reach every child. Many children then undo them: `extracted`, `OffSpill` and `Run1B` replace the fits and the
  path, `from_rec-combined` turns helices off again, and `from_rec-crv-kpp-MC` empties the path.
  [#442](https://github.com/Mu2e/EventNtuple/issues/442) is the same problem: `cosmicLivetime` was turned off in
  `from_mcs-mockdata.fcl`, so it was off in nearly every job.
- **Overrides by list index** (`trk.fits[0].mvaTags`, `fits[0].trkDtDtTag`) break silently if the base changes
  its list of fits.
- **File names don't show what a job depends on:** the `from_rec-*` jobs inherit from `from_mcs-*` jobs, and
  four `from_dig-*` jobs and `TrkAnaLineFromDigis.fcl` load the validation database
  (`Production/Validation/database.fcl`). Only the DeCalib jobs say so in a comment.

### Stale documentation and comments

- **`fcl/README.md`** names `from_mcs-DeMCalib.fcl` and `from_dig-DeMCalib.fcl` (the files are `*-DeCalib.fcl`).
  It is missing 12 files: `CompareDeTracks`, `TrkAnaLineFromDigis`, `from_dig-calo`, `from_dig-OnSpill`,
  `from_dig-DeCalib_seed`, `from_mcs-mockdata_crvInf`, `from_mcs-RefSelect`, both `RefExtrapSelect` files,
  `from_mcs-reflection`, `from_rec-combined` and `from_rec-crv-kpp-MC`. It also still says the mock-data job
  "removes `genCountLogger`".
- **`from_mcs-Run1B.fcl`**:
  - ends with a comment "add back genCountLogger" that no longer matches the line under it;
  - sets `trkDtDtTag : "TrkDtDt:MergeKKLines"` while `fillTrkDtDt` is false.
- **`from_dig-OnSpill.fcl`**:
  - sets a top-level `end_paths` that art ignores (art falls back to running every end path, so the job still
    works);
  - keeps the old TrkAna process and output names.
- **`AGENTS.md`** refers to `validation/test_fcls.sh`; the script is `validation/test_fcls.py`.

## Questions for the owners before consolidating

1. Which of these FCLs do production and other repositories use? Their names and outputs need to stay stable.
   This was not surveyed here.
2. Is `from_mcs-reflection.fcl` superseded by `from_mcs-RefSelect.fcl` and the `RefExtrapSelect` pair, and do
   the latter two need separate e and mu files? (David Brown)
3. Is `TrkAnaLineFromDigis.fcl` still needed, or does `from_mcs-extracted.fcl` on reconstructed files cover it?
4. Should `from_mcs-mixed.fcl` stay as a name for `from_mcs-primary.fcl`?
5. Should the example FCLs stay in `fcl/` or move to a separate examples directory, and should they be run by
   `validation/commands.txt`?
6. Which Production version must each `from_dig-*` and `RefSelect` job work with?

## Method

- **Include tree, overrides and inputs**: read from each file on `main`.
- **Authors and dates**: from `git log` on `main`, counting commits per author for each file.
- **Validation**: whether the file appears in `validation/commands.txt`.
- **Parse check**: `fhicl-dump` on every job FCL, with a muse setup of this workspace and Production `main`
  (`bcfef276`) first on `FHICL_FILE_PATH`.
