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
  { leafname : ""           inputTag : "TrkQualAll:ANN"       modelVersion : "TrkQual_ANN1_v2.0" },
  { leafname : "_candidate" inputTag : "TrkQualCandidate:ANN" modelVersion : "TrkQual_ANN1_v3_rc1" }
]
```

This writes `trkqual` and `trkqual_candidate`. EventNtuple also records the
track branch, output branch, input tag, and model version in
`EventNtuple/trkqual_metadata`; `checkEventNtuple` prints this metadata.

## Calorimeter entrant truth

`calohitsmc.entrantSimIds` records the calo-entrant SimParticle ID for each
energy deposit, in the same order as `calohitsmc.simParticleIds`. A value of
`-1` means the producer could not resolve an entrant. The column is empty by
default; existing configurations do not need an entrant product.

To fill it, use an Offline release containing `CaloEntrantTruthMaker`
([Offline #1911](https://github.com/Mu2e/Offline/pull/1911)) and configure:

```fcl
physics.producers.CaloEntrantTruthMaker : {
  module_type : CaloEntrantTruthMaker
  caloHitMCTag : "compressRecoMCs"
  # These two tags support legacy inputs without CaloHitMC crystal IDs.
  caloClusterTag : "CaloClusterMaker"
  caloClusterMCTag : "compressRecoMCs"
}
physics.analyzers.EventNtuple.calo.mc.entrantTag : "CaloEntrantTruthMaker"
```

Add `CaloEntrantTruthMaker` to the job's producer path before the EventNtuple
analyzer runs. Its `caloHitMCTag` must select the same collection as
`EventNtuple.calo.mc.hitMCTag`. Both the global `mc.fill` switch and
calorimeter MC hit filling must be enabled.
If the input already contains the entrant product, set `entrantTag` to that
product's input tag instead of scheduling another producer.

When enabled, missing products, collection-size or deposit-count mismatches,
and entries referring to a different MC hit cause an exception. The alignment
is within `calohitsmc`; matching reconstructed `calohits` rows to MC rows is a
separate operation.

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
