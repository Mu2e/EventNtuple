//
// Class for help filling Info structs
// Original author: A. Edmonds (November 2018)
//
#ifndef EventNtuple_InfoStructHelper_hh
#define EventNtuple_InfoStructHelper_hh
#include "Offline/RecoDataProducts/inc/StrawHitIndex.hh"
#include "Offline/RecoDataProducts/inc/KalSeed.hh"
#include "Offline/RecoDataProducts/inc/StrawHitFlag.hh"
#include "Offline/RecoDataProducts/inc/RecoCount.hh"
#include "Offline/RecoDataProducts/inc/HelixSeed.hh"
#include "Offline/RecoDataProducts/inc/CrvCoincidenceCluster.hh"
#include "Offline/RecoDataProducts/inc/MVAResult.hh"
#include "Offline/BFieldGeom/inc/BFieldManager.hh"
#include "Offline/GeometryService/inc/DetectorSystem.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"
#include "Offline/CalorimeterGeom/inc/DiskCalorimeter.hh"
#include "Offline/RecoDataProducts/inc/CaloCluster.hh"

#include "EventNtuple/inc/HitCount.hh"
#include "EventNtuple/inc/TrkInfo.hh"
#include "EventNtuple/inc/TrkSegInfo.hh"
#include "EventNtuple/inc/LoopHelixInfo.hh"
#include "EventNtuple/inc/CentralHelixInfo.hh"
#include "EventNtuple/inc/KinematicLineInfo.hh"
#include "EventNtuple/inc/TrkStrawHitInfo.hh"
#include "EventNtuple/inc/TrkStrawHitCalibInfo.hh"
#include "EventNtuple/inc/TrkStrawMatInfo.hh"
#include "EventNtuple/inc/TrkCaloHitInfo.hh"
#include "EventNtuple/inc/TrkPIDInfo.hh"
#include "EventNtuple/inc/HelixInfo.hh"
#include "EventNtuple/inc/MVAResultInfo.hh"
#include "EventNtuple/inc/CrvHitInfoReco.hh"
#include "EventNtuple/inc/CaloClusterInfo.hh"
#include "EventNtuple/inc/CaloHitInfo.hh"
#include "EventNtuple/inc/CaloRecoDigiInfo.hh"
#include "EventNtuple/inc/CaloDigiInfo.hh"
#include "art/Framework/Principal/Handle.h"
#include <vector>
#include <functional>
namespace mu2e {
  class InfoStructHelper {

  private:
    double _bz0;

  public:
    InfoStructHelper() {}

    void updateSubRun() {
      mu2e::GeomHandle<mu2e::BFieldManager> bfmgr;
      mu2e::GeomHandle<mu2e::DetectorSystem> det;
      CLHEP::Hep3Vector vpoint_mu2e = det->toMu2e(CLHEP::Hep3Vector(0.0,0.0,0.0));
      _bz0 = bfmgr->getBField(vpoint_mu2e).z();
    }

    void fillHitCount(const StrawHitFlagCollection& flags, HitCount& hitcount);
    void fillHitCount(RecoCount const& nrec, HitCount& hitcount);

    void fillTrkInfo(const KalSeed& kseed,std::vector<TrkInfo>& trkinfo);
    void fillTrkSegInfo(const KalSeed& kseed,std::vector<std::vector<TrkSegInfo>>& trkseginfos);
    void fillTrkInfoHits(const KalSeed& kseed,TrkInfo& trkinfo);
    void fillTrkInfoStraws(const KalSeed& kseed,TrkInfo& trkinfo);
    void fillLoopHelixInfo(const KalSeed& kseed, std::vector<std::vector<LoopHelixInfo>>& lhis);
    void fillCentralHelixInfo(const KalSeed& kseed, std::vector<std::vector<CentralHelixInfo>>& chis);
    void fillKinematicLineInfo(const KalSeed& kseed, std::vector<std::vector<KinematicLineInfo>>& klis);
    void fillTrkQualInfo(const KalSeed& kseed, MVAResult mva, std::vector<MVAResultInfo>& all_mvas);
    void fillHitInfo(const KalSeed& kseed, std::vector<std::vector<TrkStrawHitInfo>>& all_tshinfos, std::vector<std::vector<TrkStrawHitCalibInfo>>& all_tshcinfos, bool saveCalib );
    void fillMatInfo(const KalSeed& kseed, std::vector<std::vector<TrkStrawMatInfo>>& tminfos );
    void fillTrkCaloHitInfo(const KalSeed& kseed, std::vector<TrkCaloHitInfo>& tchinfo );
    void fillTrkPIDInfo(const TrkCaloHitPID& tchp, const KalSeed& kseed, TrkPIDInfo& trkpidInfo);
    void fillHelixInfo(art::Ptr<HelixSeed> const& hptr, std::vector<HelixInfo>& all_hinfos);

    void fillCaloClusterInfo(const CaloCluster& ccptr, std::vector<CaloClusterInfo>& clusterinfos);
    void fillCaloHitInfo(const CaloHit& chptr, std::vector<CaloHitInfo>& hitinfos, int clusterIdx = -1);
    void fillCaloRecoDigiInfo(const CaloRecoDigi& crdptr, std::vector<CaloRecoDigiInfo>& recodigiinfos, int hitIdx = -1);
    void fillCaloDigiInfo(const CaloDigi& cdptr, std::vector<CaloDigiInfo>& digiinfos, int recodigiIdx = -1);
  };
}

#endif
