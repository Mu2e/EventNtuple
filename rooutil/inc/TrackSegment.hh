#ifndef TrackSegment_hh_
#define TrackSegment_hh_

#include <functional>
#include "EventNtuple/inc/TrkSegInfo.hh"
#include "EventNtuple/inc/SurfaceStepInfo.hh"
#include "EventNtuple/inc/LoopHelixInfo.hh"
#include "EventNtuple/inc/CentralHelixInfo.hh"
#include "EventNtuple/inc/KinematicLineInfo.hh"

namespace rooutil {
  struct TrackSegment {
    TrackSegment() { }
    TrackSegment(mu2e::TrkSegInfo* trkseg) : trkseg(trkseg) { }

    // Pointers to the data
    mu2e::TrkSegInfo* trkseg = nullptr;
    mu2e::SurfaceStepInfo* trksegmc = nullptr;
    mu2e::LoopHelixInfo* trksegpars_lh = nullptr;
    mu2e::CentralHelixInfo* trksegpars_ch = nullptr;
    mu2e::KinematicLineInfo* trksegpars_kl = nullptr;

    static bool earliest(const TrackSegment& seg1, const TrackSegment& seg2) {
      return seg1.trkseg->time < seg2.trkseg->time;
    }
  };

  typedef std::function<bool(TrackSegment&)> TrackSegmentCut;
  typedef std::vector<TrackSegment> TrackSegments;
} // namespace rooutil
#endif
