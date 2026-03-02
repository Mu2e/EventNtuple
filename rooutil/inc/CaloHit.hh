#ifndef CaloHit_hh_
#define CaloHit_hh_

#include <functional>
#include "EventNtuple/inc/TrkStrawHitInfo.hh"
#include "EventNtuple/inc/TrkStrawHitInfoMC.hh"
#include "EventNtuple/inc/TrkStrawHitCalibInfo.hh"

namespace rooutil {
  struct CaloHit {
    CaloHit() { }

    // Pointers to the data
    mu2e::CaloHitInfo* reco = nullptr;
  };

  typedef std::function<bool(CaloHit&)> CaloHitCut;
  typedef std::vector<CaloHit> CaloHits;
} // namespace rooutil
#endif
