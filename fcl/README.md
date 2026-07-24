# Fcl Files
## Naming Convention
The naming convention for fcl files is:

```
from_tier-type_extra.fcl
```

where ```tier``` is the data tier of the input dataset, ```type``` is the type of dataset (e.g. primary-only, extracted position), and ```extra``` gives some extra information (optional)

## Multiple TrkQual outputs

Configure each TrkQual result in a track fit with `trkQualLeaves`. `leafname` is
appended to `<branchname>qual`, so stable output names do not depend on the
order of the configured algorithms:

```
trkQualLeaves : [
  { leafname : ""           inputTag : "TrkQualAll:ANN"       modelVersion : "TrkQual_ANN1_v2" },
  { leafname : "_candidate" inputTag : "TrkQualCandidate:ANN" modelVersion : "TrkQual_ANN1_v3_rc1" }
]
```

This writes `trkqual` and `trkqual_candidate`. EventNtuple also records the
track branch, output branch, input tag, and model version in
`EventNtuple/trkqual_metadata`; `checkEventNtuple` prints this metadata. RooUtil
can bind either output by name with `MakeTrackUserBranch<mu2e::MVAResultInfo>`.

## Table of Fcl Files

| fcl file | runs on | additional info |
|----------|-----|-----|
| from_mcs-mockdata.fcl | mock datasets | removes ```genCountLogger``` which does not apply to mock datasets|
| from_mcs-mockdata_noMC.fcl | mock datasets | doesn't include MC in output |
| from_mcs-extracted.fcl | reconstructed extracted position datasets | |
| from_mcs-primary.fcl | reconstructed primary (i.e. no background hits) datasets | |
| from_mcs-mixed.fcl | reconstructed mixed (i.e. primary+background hits) datasets | |
| from_mcs-ceSimReco.fcl | output of Production/Validation/ceSimReco.fcl | |
| from_mcs-ceSimRecoVal.fcl | output of EventNtuple/validation/ceSimReco.fcl | for validating the ```trkhitcalibs``` branch |
| from_mcs-mockdata_separateTrkBranches.fcl | mock datasets | example on how to separate the tracks into separate branches again|
| from_mcs-mockdata_selectorExample.fcl | mock datasets | example on how to use a selector to select certain types of tracks before putting them into the EventNtuple |
| from_mcs-mixed_trkQualCompare.fcl | reconstructed mixed (i.e. primary+background hits) datasets | shows explicitly named TrkQual outputs and embedded model-version provenance; requires the listed comparison ONNX models |
| from_mcs-primary_addVDSteps.fcl | reconstructed primary (i.e. no background hits) datasets | shows how to add the branch for virtual detector steps |
| from_mcs-Run1B.fcl | reconstructed Run-1B (backup plan) datasets | adds the branch for virtual detector steps |
| from_mcs-DeMCalib.fcl | reconstructed primary or mixed datasets | only writes one track per event |
| from_mcs-OffSpill.fcl | off spill datasets | only contains ```CentralHelix``` tracks (i.e. field-on cosmics) |
| from_dig-mockdata.fcl | mock datasets (digis) | runs reconstruction and creates EventNtuple in one job |
| from_dig-DeMCalib.fcl | digitized primary or mixed datasets | also runs reconstruction, only writes one track per event |
| from_rec-crv-kpp.fcl | CRV KPP Data | only contains ```evtinfo``` and ```crv*``` branches |
| from_rec-crv-kpp_withCrvDigis.fcl | CRV KPP Data | as above but with ```crvdigis``` branch adde |
