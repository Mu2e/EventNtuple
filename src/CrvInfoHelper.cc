#include "EventNtuple/inc/CrvInfoHelper.hh"

#include "Offline/CosmicRayShieldGeom/inc/CosmicRayShield.hh"
#include "Offline/CRVConditions/inc/CRVDigitizationPeriod.hh"
#include "Offline/GeometryService/inc/DetectorSystem.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"
#include "Offline/GeometryService/inc/GeometryService.hh"
#include "Offline/MCDataProducts/inc/CrvCoincidenceClusterMC.hh"
#include "Offline/MCDataProducts/inc/CrvDigiMC.hh"
#include "Offline/MCDataProducts/inc/CrvStep.hh"
#include "Offline/MCDataProducts/inc/MCTrajectory.hh"
#include "Offline/MCDataProducts/inc/MCRelationship.hh"
#include "Offline/RecoDataProducts/inc/CrvCoincidenceCluster.hh"
#include "Offline/RecoDataProducts/inc/CrvDigi.hh"
#include "Offline/RecoDataProducts/inc/CrvRecoPulse.hh"
#include "Offline/DataProducts/inc/PDGCode.hh"
#include "art/Framework/Principal/Handle.h"
#include "Offline/CRVResponse/inc/CrvMCHelper.hh"
#include "Offline/CRVReco/inc/CrvHelper.hh"
#include "Offline/GeometryService/inc/DetectorSystem.hh"

namespace mu2e
{
  void CrvInfoHelper::FillCrvHitInfoCollections(
      art::Handle<CrvCoincidenceClusterCollection> const& crvCoincidences,
      art::Handle<CrvCoincidenceClusterMCCollection> const& crvCoincidencesMC,
      art::Handle<CrvRecoPulseCollection> const& crvRecoPulses,
      art::Handle<CrvStepCollection> const& crvSteps,
      art::Handle<MCTrajectoryCollection> const& mcTrajectories,
      CrvHitInfoRecoCollection &recoInfo, CrvHitInfoMCCollection &MCInfo,
      CrvSummaryReco &recoSummary, CrvSummaryMC &MCSummary,
      CrvPlaneInfoMCCollection &MCInfoPlane, double crvPlaneY,
      art::Handle<PrimaryParticle> const& primary) {
    GeomHandle<CosmicRayShield> CRS;
    GeomHandle<DetectorSystem> tdet;

    if(!crvCoincidences.isValid()) return;
    size_t nClusters=crvCoincidences->size();
    for(size_t i=0; i<nClusters; i++)
    {
      const CrvCoincidenceCluster &cluster = crvCoincidences->at(i);
      const std::vector<art::Ptr<CrvRecoPulse> > coincRecoPulses_ = cluster.GetCrvRecoPulses(); // Get the reco pulses from the coincidence 
      // Initiliase PEs per layer
      std::array<float, CRVId::nLayers> PEsPerLayer_ = {0.}; 
      // Initiliase PEs per layer per side
      std::array<float, CRVId::nLayers * CRVId::nSidesPerBar> sidePEsPerLayer_ = {0.};
      for(size_t j=0; j<coincRecoPulses_.size(); j++) // Loop through the pulses
      {
        // Skip duplicate pulses (those with multiple peaks)
        if(coincRecoPulses_.at(j)->GetRecoPulseFlags().test(CrvRecoPulseFlagEnums::duplicateNoFitPulse)) continue;
        // Get PEs associated with this reco pulse
        float PEs = coincRecoPulses_.at(j)->GetPEsNoFit();
        // Get layer number from the bar index associated with this reco pulse
        const CRSScintillatorBarIndex &crvBarIndex = coincRecoPulses_.at(j)->GetScintillatorBarIndex(); 
        const CRSScintillatorBar &crvCounter = CRS->getBar(crvBarIndex);
        const CRSScintillatorBarId &crvCounterId = crvCounter.id();
        int layerNumber = crvCounterId.getLayerNumber();
        // Get the side number 
        // The negative side has SiPM indices 0 and 2, the postive side has indices 1 and 3.
        // zero/one index indicates negative/positive; negative/positive indicates direction wrt the axis in the coordinate system.
        int side = coincRecoPulses_.at(j)->GetSiPMNumber() % CRVId::nSidesPerBar; 
        // Sum PEs for this coincidence, indexed by layer number
        PEsPerLayer_[layerNumber] += PEs; 
        // Sum PEs for this coincidence, indexed by layer number and side number
        int layerSideIndex = layerNumber * CRVId::nSidesPerBar + side; // Indices for a flattened 2D matrix: layers (rows), sides (columns)
        sidePEsPerLayer_[layerSideIndex] += PEs;
      }

      //fill the Reco collection
      recoInfo.emplace_back(
          cluster.GetCrvSectorType(),
          tdet->toDetector(cluster.GetAvgHitPos()),
          cluster.GetStartTime(), cluster.GetEndTime(), cluster.GetAvgHitTime(),
          cluster.GetPEs(),
          PEsPerLayer_, // PEsPerLayer array is not a member of the mu2e::CrvCoincidenceCluster class
          sidePEsPerLayer_, // ""
          cluster.GetCrvRecoPulses().size(),
          cluster.GetLayers().size(),
          cluster.GetSlope());
    }

    if(!crvRecoPulses.isValid()) return;
    size_t nRecoPulses=crvRecoPulses->size();
    recoSummary.totalPEs=0;
    std::set<CRSScintillatorBarIndex> counters;
    for(size_t i=0; i<nRecoPulses; i++)
    {
      recoSummary.totalPEs+=crvRecoPulses->at(i).GetPEs();
      counters.insert(crvRecoPulses->at(i).GetScintillatorBarIndex());
    }
    recoSummary.nHitCounters=counters.size();


    //fill the MC collection
    if(crvCoincidencesMC.isValid())
    {
      size_t nClustersMC=crvCoincidencesMC->size();
      if(nClusters!=nClustersMC) std::cout<<"The number of MC and reco CRV coincidence clusters does not match!"<<std::endl;
      for(size_t i=0; i<nClustersMC; i++)
      {
        const CrvCoincidenceClusterMC &clusterMC = crvCoincidencesMC->at(i);
        if(clusterMC.HasMCInfo())
        {
          const art::Ptr<SimParticle> &simParticle = clusterMC.GetMostLikelySimParticle();
          const art::Ptr<SimParticle> &primaryParticle = FindPrimaryParticle(simParticle);
          const art::Ptr<SimParticle> &parentParticle  = FindParentParticle(simParticle);
          const art::Ptr<SimParticle> &gparentParticle = FindGParentParticle(simParticle);
          MCInfo.emplace_back(
              true,
              simParticle->pdgId(),
              primaryParticle->pdgId(),
              primaryParticle->startMomentum().e() - primaryParticle->startMomentum().m(),
              tdet->toDetector(primaryParticle->startPosition()),
              parentParticle->pdgId(),
              parentParticle->startMomentum().e()  - parentParticle->startMomentum().m(),
              tdet->toDetector(parentParticle->startPosition()),
              gparentParticle->pdgId(),
              gparentParticle->startMomentum().e() - gparentParticle->startMomentum().m(),
              tdet->toDetector(gparentParticle->startPosition()),
              tdet->toDetector(clusterMC.GetAvgHitPos()),
              clusterMC.GetAvgHitTime(),
              clusterMC.GetTotalEnergyDeposited());
        }
        else MCInfo.emplace_back();
      }
    }

    MCSummary = CrvSummaryMC();
    if(crvSteps.isValid())
    {
      size_t nSteps=crvSteps->size();
      MCSummary.totalEnergyDeposited=0;
      std::set<CRSScintillatorBarIndex> counters;
      double totalStep[] = {0, 0, 0, 0};
      for(size_t i=0; i<nSteps; i++){
        MCSummary.totalEnergyDeposited+=crvSteps->at(i).visibleEDep();
        counters.insert(crvSteps->at(i).barIndex());
        const CRSScintillatorBarId &CRVCounterId = CRS->getBar(crvSteps->at(i).barIndex()).id();
        int layer = CRVCounterId.getLayerNumber();
        int pdgId = crvSteps->at(i).simParticle()->pdgId();
        if(abs(pdgId)==PDGCode::mu_minus)
          totalStep[layer] = totalStep[layer] + crvSteps->at(i).pathLength();

        // Save info from the first step in the CRV
        if(i==0){
          CLHEP::Hep3Vector CrvPos = crvSteps->at(i).startPosition();
          MCSummary.pos = XYZVectorF(tdet->toDetector(CrvPos));
          int sectorNumber = CRVCounterId.getShieldNumber();
          MCSummary.sectorNumber = sectorNumber;
          MCSummary.sectorType = CRS->getCRSScintillatorShield(sectorNumber).getSectorType();
          MCSummary.pdgId = pdgId;
        }
      }

      MCSummary.nHitCounters=counters.size();
      MCSummary.minPathLayer=*std::min_element(totalStep,totalStep+4);
      MCSummary.maxPathLayer=*std::max_element(totalStep,totalStep+4);
    }

    //locate points where the cosmic MC trajectories cross the xz plane of CRV-T
    if(mcTrajectories.isValid())
    {
      auto bestprimarysp = primary->primarySimParticles().front();
      for(auto trajectoryIter=mcTrajectories->begin(); trajectoryIter!=mcTrajectories->end(); trajectoryIter++)
      {
        const art::Ptr<SimParticle> &trajectorySimParticle = trajectoryIter->first;

        // We want to store the MC-truth information for the cosmic primary that crossed the plane
        //
        // Sometimes we have e.g. a neutron that undergoes a neutronInelasic process and "produces" a neutron
        // that crosses the plane.In this case, the primary particle neutron and the "daughter" neutron
        // are not the same SimParticle but here we still want to record the information of the "daughter" neutron
        MCRelationship rel(trajectorySimParticle, bestprimarysp);
        if(rel != MCRelationship::same && trajectorySimParticle->pdgId() != bestprimarysp->pdgId()) { continue; }

        const art::Ptr<SimParticle> &trajectoryPrimaryParticle = FindPrimaryParticle(trajectorySimParticle);
        GenId genId = trajectoryPrimaryParticle->genParticle()->generatorId();
        if(genId.isCosmic())
        {
          const std::vector<MCTrajectoryPoint> &points = trajectoryIter->second.points();
          if(points.size()<1) continue;
          CLHEP::Hep3Vector previousPos=points[0].pos();
          for(size_t i=1; i<points.size(); i++)
          {
            CLHEP::Hep3Vector pos=points[i].pos();
            if((previousPos.y()>crvPlaneY && pos.y()<=crvPlaneY) || (previousPos.y()<crvPlaneY && pos.y()>=crvPlaneY))
            {
              double fraction=(crvPlaneY-pos.y())/(previousPos.y()-pos.y());
              CLHEP::Hep3Vector planePos=fraction*(previousPos-pos)+pos;
              CLHEP::Hep3Vector planeDir=(pos-previousPos).unit();
              double planeTime=fraction*(points[i-1].t()-points[i].t())+points[i].t();
              double planeKineticEnergy=fraction*(points[i-1].kineticEnergy()-points[i].kineticEnergy())+points[i].kineticEnergy();
              MCInfoPlane.emplace_back(trajectorySimParticle->pdgId(),
                  trajectoryPrimaryParticle->pdgId(),
                  trajectoryPrimaryParticle->startMomentum().e(),
                  tdet->toDetector(trajectoryPrimaryParticle->startPosition()),
                  tdet->toDetector(planePos),
                  planeDir,
                  planeTime,
                  planeKineticEnergy,
                  0);  //unused
            }
            previousPos=pos;
          }
        }
      }
    }

  }//FillCrvInfoStructure

  void CrvInfoHelper::FillCrvPulseInfoCollections (
      art::Handle<CrvRecoPulseCollection> const& crvRecoPulses,
      art::Handle<CrvDigiMCCollection> const& crvDigiMCs,
      CrvPulseInfoRecoCollection &recoInfo, CrvHitInfoMCCollection &MCInfo){
    GeomHandle<DetectorSystem> tdet;

    if(!crvRecoPulses.isValid()) return;

    GeomHandle<CosmicRayShield> CRS;
    const std::map<int,int> sipm_map = GetSiPMMap(CRS);

    // Loop through all reco pulses
    for(size_t recoPulseIndex=0; recoPulseIndex<crvRecoPulses->size(); recoPulseIndex++)
    {
      const art::Ptr<CrvRecoPulse> crvRecoPulse(crvRecoPulses, recoPulseIndex);
      //get information about the counter
      const CRSScintillatorBarIndex &barIndex = crvRecoPulse->GetScintillatorBarIndex();
      int sectorNumber  = -1;
      int moduleNumber  = -1;
      int layerNumber   = -1;
      int counterNumber = -1;
      CrvHelper::GetCrvCounterInfo(CRS, barIndex, sectorNumber, moduleNumber, layerNumber, counterNumber);

      //Reco pulses information
      int SiPM = crvRecoPulse->GetSiPMNumber();
      int SiPMId = sipm_map.find(barIndex.asInt()*CRVId::nChanPerBar + SiPM)->second;
      CLHEP::Hep3Vector HitPos = CrvHelper::GetCrvCounterPos(CRS, barIndex);
      recoInfo.emplace_back(HitPos, barIndex.asInt(), sectorNumber, SiPMId,
          crvRecoPulse->GetPEs(), crvRecoPulse->GetPEsPulseHeight(), crvRecoPulse->GetPulseHeight(),
          crvRecoPulse->GetPulseBeta(), crvRecoPulse->GetPulseFitChi2(), crvRecoPulse->GetPulseTime());

      //MCtruth pulses information
      double visibleEnergyDeposited  = 0;
      double earliestHitTime         = 0;
      double avgHitTime         = 0;
      CLHEP::Hep3Vector earliestHitPos;
      CLHEP::Hep3Vector avgHitPos;
      art::Ptr<SimParticle> mostLikelySimParticle;
      //for this reco pulse
      CrvMCHelper::GetInfoFromCrvRecoPulse(crvRecoPulse, crvDigiMCs, visibleEnergyDeposited,earliestHitTime, earliestHitPos, avgHitTime, avgHitPos,
          mostLikelySimParticle);

      bool hasMCInfo = (mostLikelySimParticle.isNonnull()?true:false); //MC
      if(hasMCInfo)
      {
        const art::Ptr<SimParticle> &primaryParticle = FindPrimaryParticle(mostLikelySimParticle);
        const art::Ptr<SimParticle> &parentParticle = FindParentParticle(mostLikelySimParticle);
        const art::Ptr<SimParticle> &gparentParticle = FindGParentParticle(mostLikelySimParticle);
        MCInfo.emplace_back(true, mostLikelySimParticle->pdgId(),
            primaryParticle->pdgId(),
            primaryParticle->startMomentum().e() - primaryParticle->startMomentum().m(),
            tdet->toDetector(primaryParticle->startPosition()),
            parentParticle->pdgId(),
            parentParticle->startMomentum().e()  - parentParticle->startMomentum().m(),
            tdet->toDetector(parentParticle->startPosition()),
            gparentParticle->pdgId(),
            gparentParticle->startMomentum().e() - gparentParticle->startMomentum().m(),
            tdet->toDetector(gparentParticle->startPosition()),
            tdet->toDetector(avgHitPos),
            avgHitTime,
            visibleEnergyDeposited);
      }
      else
        MCInfo.emplace_back();
    }

  } //FillCrvPulseInfoCollections

  // Fill digis struct 
  void CrvInfoHelper::FillCrvDigiInfoCollections (
      art::Handle<CrvRecoPulseCollection> const& crvRecoPulses,
      art::Handle<CrvDigiCollection> const& crvDigis,
      CrvWaveformInfoCollection &digiInfo){

    if(!crvRecoPulses.isValid()) return;

    GeomHandle<CosmicRayShield> CRS;
    const std::map<int,int> sipm_map = GetSiPMMap(CRS);

    // Fill digis/waveforminfo struct
    for(size_t j=0; j<crvDigis->size(); j++)
    {
      mu2e::CrvDigi const& digi(crvDigis->at(j));
      int SiPMId = sipm_map.find(digi.GetScintillatorBarIndex().asInt()*4 + digi.GetSiPMNumber())->second;
      for(size_t k=0; k<digi.GetADCs().size(); k++)
        digiInfo.emplace_back(digi.GetADCs()[k], (digi.GetStartTDC()+k)*CRVDigitizationPeriod, SiPMId);
    }
  } // FillCrvDigiInfoCollections

}
