#ifndef TrackHit_hh_
#define TrackHit_hh_

#include <functional>
#include "EventNtuple/inc/TrkStrawHitInfo.hh"
#include "EventNtuple/inc/TrkStrawHitInfoMC.hh"
#include "EventNtuple/inc/TrkStrawHitCalibInfo.hh"

struct TrackHit {
  TrackHit() { }

  // Pointers to the data
  mu2e::TrkStrawHitInfo* reco = nullptr;
  mu2e::TrkStrawHitInfoMC* mc = nullptr;
  mu2e::TrkStrawHitCalibInfo* calib = nullptr;
};

typedef std::function<bool(TrackHit&)> TrackHitCut;
typedef std::vector<TrackHit> TrackHits;

#endif
