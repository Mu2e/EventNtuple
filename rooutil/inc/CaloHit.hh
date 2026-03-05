#ifndef CaloHit_hh_
#define CaloHit_hh_

#include <functional>
#include "EventNtuple/inc/CaloHitInfo.hh"
#include "EventNtuple/inc/CaloHitInfoMC.hh"

namespace rooutil {
  struct CaloHit {
    CaloHit() { }

    // Pointers to the data
    mu2e::CaloHitInfo* reco = nullptr;
    mu2e::CaloHitInfoMC* mc = nullptr;
  };

  typedef std::function<bool(CaloHit&)> CaloHitCut;
  typedef std::vector<CaloHit> CaloHits;
} // namespace rooutil
#endif
