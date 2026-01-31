filename="\"nts.ntuple.after.root\""

scripts=( "PrintEvents.C" "CreateNtuple.C" "CreateTrackNtuple.C" "PlotCRVPEs.C" "PlotCRVPEsVsMCEDep.C" "PlotEntranceFitPars.C" "PlotEntranceMomentum.C"
          "PlotEntranceMomentumCRVCut.C" "PlotEntranceMomentumResolution.C" "PlotEntranceMomentumResolution_TrkQualCut.C"
          "PlotMCParentPosZ.C" "PlotMCParticleMom.C" "PlotMuonPosZ.C" "PlotStoppingTargetFoilSegment.C" "PlotTrackNHits_RecoVsTrue.C"
          "PlotTrkCaloHitEnergy.C" "PrintEventsNoMC.C" "TrackCounting.C" "PlotTrackHitTimes.C" "PlotTrackHitTimesMC.C" "PlotStrawMaterials.C"
          "PlotGenCosmicMom.C" "PlotCRVTotalPEs.C" "PlotEntranceMomentum_UpstreamDownstream.C" "PlotVDSteps.C" )

for script in "${scripts[@]}"
do
    root -l -b -q rooutil/examples/${script}++\(${filename}\)

    if [ $? != 0 ]; then # quit as soon as one script fails
        echo $script " failed"
        break;
    fi
done

# Check the that reduced ntuple runs in RooUtil
filename="\"example_ntuple.root\""
root -l -b -q rooutil/examples/PrintEvents.C++\(${filename}\)
