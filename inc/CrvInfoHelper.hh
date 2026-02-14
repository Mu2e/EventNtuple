//
//  Replacement for CRVAnalysis
//
#include "EventNtuple/inc/CrvHitInfoReco.hh"
#include "EventNtuple/inc/CrvHitInfoMC.hh"
#include "EventNtuple/inc/CrvWaveformInfo.hh"
#include "EventNtuple/inc/CrvSummaryReco.hh"
#include "EventNtuple/inc/CrvSummaryMC.hh"
#include "EventNtuple/inc/CrvPlaneInfoMC.hh"
#include "EventNtuple/inc/CrvPulseInfoReco.hh"
#include "Offline/DataProducts/inc/CRVId.hh"
#include "Offline/RecoDataProducts/inc/CrvCoincidenceCluster.hh"
#include "Offline/RecoDataProducts/inc/CrvRecoPulse.hh"
#include "Offline/RecoDataProducts/inc/CrvDigi.hh"
#include "Offline/MCDataProducts/inc/CrvDigiMC.hh"
#include "Offline/MCDataProducts/inc/CrvStep.hh"
#include "Offline/MCDataProducts/inc/SimParticle.hh"
#include "Offline/MCDataProducts/inc/CrvCoincidenceClusterMC.hh"
#include "Offline/MCDataProducts/inc/MCTrajectory.hh"
#include "Offline/MCDataProducts/inc/PrimaryParticle.hh"
#include "Offline/CosmicRayShieldGeom/inc/CosmicRayShield.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"
#include "art/Framework/Principal/Handle.h"
#include "Offline/DataProducts/inc/EventWindowMarker.hh"

namespace mu2e
{
  class CrvInfoHelper
  {
    public:

      CrvInfoHelper() {}

      void FillCrvHitInfoCollections(
          art::Handle<CrvCoincidenceClusterCollection> const& crvCoincidences,
          art::Handle<CrvCoincidenceClusterMCCollection> const& crvCoincidencesMC,
          art::Handle<CrvRecoPulseCollection> const& crvRecoPulses,
          art::Handle<CrvStepCollection> const& crvSteps,
          art::Handle<MCTrajectoryCollection> const& mcTrajectories,
          CrvHitInfoRecoCollection &recoInfo, CrvHitInfoMCCollection &MCInfo,
          CrvSummaryReco &recoSummary, CrvSummaryMC &MCSummary,
          CrvPlaneInfoMCCollection &MCInfoPlane, double crvPlaneY,
          art::Handle<PrimaryParticle> const& primary,
          bool crvNoFitReco);

      void FillCrvPulseInfoCollections(
          art::Handle<CrvRecoPulseCollection> const& crvRecoPulses,
          art::Handle<CrvDigiMCCollection> const& crvDigiMCs,
          art::Handle<EventWindowMarker> const& ewmh,
          CrvPulseInfoRecoCollection &recoInfo, CrvHitInfoMCCollection &MCInfo,
          bool crvNoFitReco);

      void FillCrvDigiInfoCollections(
          art::Handle<CrvRecoPulseCollection> const& crvRecoPulses,
          art::Handle<CrvDigiCollection> const& crvDigis,
          CrvWaveformInfoCollection &digiInfo);

    private:
      static const art::Ptr<SimParticle> &FindPrimaryParticle(const art::Ptr<SimParticle> &simParticle)
      {
        return simParticle->hasParent() ? FindPrimaryParticle(simParticle->parent()) : simParticle;
      }
      static const SimParticle &FindPrimaryParticle(const SimParticle &simParticle)
      {
        return simParticle.hasParent() ? *FindPrimaryParticle(simParticle.parent()) : simParticle;
      }
      static const art::Ptr<SimParticle> &FindParentParticle(const art::Ptr<SimParticle> &simParticle)
      {
        return simParticle->hasParent() ? simParticle->parent() : simParticle;
      }
      static const art::Ptr<SimParticle> &FindGParentParticle(const art::Ptr<SimParticle> &simParticle)
      {
        const art::Ptr<SimParticle> &parentParticle = simParticle->hasParent() ? simParticle->parent() : simParticle;
        return parentParticle->hasParent() ? parentParticle->parent() : parentParticle;
      }
      static const std::map<int,int> GetSiPMMap(GeomHandle<CosmicRayShield> &CRS) // Helper function for FillCrvDigiInfoCollections and FillCrvPulseInfoCollections
      {
        // Create SiPM map to extract sequantial SiPM IDs
        const std::vector<std::shared_ptr<CRSScintillatorBar> > &counters = CRS->getAllCRSScintillatorBars();
        std::vector<std::shared_ptr<CRSScintillatorBar> >::const_iterator iter;
        int iSiPM = 0;
        std::map<int,int> sipm_map;
        for(iter=counters.begin(); iter!=counters.end(); iter++)
        {
          const CRSScintillatorBarIndex &barIndex = (*iter)->index();
          for(size_t SiPM=0; SiPM<CRVId::nChanPerBar; SiPM++)
          {
            if(!(*iter)->getBarDetail().hasCMB(SiPM%2)) continue;
            sipm_map[barIndex.asInt()*CRVId::nChanPerBar + SiPM] = iSiPM;
            iSiPM++;
          }
        }
        return sipm_map;
      }
      static const int _trajectorySimParticleId = 300001;  //only temporarily here for some tests
  };

}
