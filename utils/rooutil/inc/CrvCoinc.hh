#ifndef CrvCoinc_hh_
#define CrvCoinc_hh_

#include <functional>
#include "EventNtuple/inc/CrvHitInfoReco.hh"
#include "EventNtuple/inc/CrvHitInfoMC.hh"

struct CrvCoinc {
  CrvCoinc(mu2e::CrvHitInfoReco* reco)
    : reco(reco) {

  }

  // Pointers to the data
  mu2e::CrvHitInfoReco* reco = nullptr;
  mu2e::CrvHitInfoMC* mc = nullptr;
};

typedef std::function<bool(const CrvCoinc&)> CrvCoincCut;
typedef std::vector<CrvCoinc> CrvCoincs;

#endif
