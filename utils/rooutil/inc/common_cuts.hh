//
// This file contains common cut functions that you might want to use.
// You can use these in your own functions too
//

#ifndef common_cuts_hh_
#define common_cuts_hh_

#include "EventNtuple/utils/rooutil/inc/Track.hh"
#include "EventNtuple/utils/rooutil/inc/TrackSegment.hh"
#include "EventNtuple/utils/rooutil/inc/CrvCoinc.hh"

//+ Track Segment Cuts - Directions
bool is_downstream(const TrackSegment& segment) { // track fit segment is going in +z direction
  // the sign of p_z tells us whether this segment is going upstream or downstream
  if (segment.trkseg->mom.z() > 0) {
      return true;
  }
  else { return false; }
}

bool is_upstream(const TrackSegment& segment) { // track fit segment is going in -z direction
  // the sign of p_z tells us whether this segment is going upstream or downstream
  if (segment.trkseg->mom.z() < 0) {
    return true;
  }
  else { return false; }
}


//+ Track Segment Cuts - Locations
bool tracker_entrance(const TrackSegment& segment) { // track fit segment is at the tracker entrance
  if (segment.trkseg->sid==mu2e::SurfaceIdDetail::TT_Front) { return true; }
  else { return false; }
}

bool tracker_middle(const TrackSegment& segment) {  // track fit segment is at the middle of the tracker
  if (segment.trkseg->sid==mu2e::SurfaceIdDetail::TT_Mid) { return true; }
  else { return false; }
}

bool tracker_exit(const TrackSegment& segment) { // track fit segment is at the tracker exit
  if (segment.trkseg->sid==mu2e::SurfaceIdDetail::TT_Back) { return true; }
  else { return false; }
}
//+ Track Segment Cuts - Other
bool has_mc_step(const TrackSegment& segment) { // track fit segment has an MC-truth SurfaceStep
  if (segment.trksegmc != nullptr) { return true; }
  else { return false; }
}


//+ Track Cuts - Particle Types
bool is_e_minus(const Track& track) { // track fit used e-minus hypothesis
  if (track.trk->pdg==11) { return true; }
  else { return false; }
}

bool is_e_plus(const Track& track) { // track fit used e-plus hypothesis
  if (track.trk->pdg==-11) { return true; }
  else { return false; }
}

bool is_mu_minus(const Track& track) { // track fit used mu-minus hypothesis
  if (track.trk->pdg==13) { return true; }
  else { return false; }
}

bool is_mu_plus(const Track& track) { // track fit used mu-plus hypothesis
  if (track.trk->pdg==-13) { return true; }
  else { return false; }
}

//+ Track Cuts - Other
bool is_reflected(const Track& track) { // track is refelected (i.e. has both an upstream and downstream fit segment at the tracker entrance)
  bool have_upstream = false;
  bool have_downstream = false;
  for (const auto& segment : track.segments) {
    if (tracker_entrance(segment)) {
      if (is_upstream(segment)) {
        have_upstream = true;
      }
      if (is_downstream(segment)) {
        have_downstream = true;
      }
    }
  }
  if (have_upstream && have_downstream) {
    return true;
  }
  else {
    return false;
  }
}

//+ CrvCoinc Cuts
bool three_of_four_coinc(const CrvCoinc& crv_coinc) { // CRV coincidence has exactly three layers hit
  if (crv_coinc.reco->nLayers == 3) { return true; }
  else { return false; }
}

//+ Combined Track & CrvCoinc Cuts
bool track_crv_coincidence(const TrackSegment& segment, const CrvCoinc& crv_coinc) { // time difference between track segment and crv_coinc is less than 250 ns
  if ( std::fabs(segment.trkseg->time - crv_coinc.reco->time) < 250) { return true; }
  else { return false; }
}

#endif
