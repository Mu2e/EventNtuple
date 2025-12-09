//
// This file contains common cut functions that you might want to use.
// You can use these in your own functions too
//

#ifndef common_cuts_hh_
#define common_cuts_hh_

#include "Offline/DataProducts/inc/PDGCode.hh"
#include "Offline/MCDataProducts/inc/ProcessCode.hh"
#include "Offline/MCDataProducts/inc/GenId.hh"
#include "Offline/MCDataProducts/inc/MCRelationship.hh"

#include "EventNtuple/rooutil/inc/Track.hh"
#include "EventNtuple/rooutil/inc/TrackSegment.hh"
#include "EventNtuple/rooutil/inc/CrvCoinc.hh"

using namespace rooutil;
//+ Track Segment Cuts - Directions
bool is_downstream(TrackSegment& segment) { // track fit segment is going in +z direction
  // the sign of p_z tells us whether this segment is going upstream or downstream
  if (segment.trkseg != nullptr) {
    if (segment.trkseg->mom.z() > 0) {
      return true;
    }
    else { return false; }
  }
  else { return false; }
}

bool is_upstream(TrackSegment& segment) { // track fit segment is going in -z direction
  // the sign of p_z tells us whether this segment is going upstream or downstream
  if (segment.trkseg != nullptr) {
    if (segment.trkseg->mom.z() < 0) {
      return true;
    }
    else { return false; }
  }
  else { return false; }
}

//+ Track Segment Cuts - Locations
bool tracker_entrance(TrackSegment& segment) { // track fit segment is at the tracker entrance
  if (segment.trkseg != nullptr) {
    if (segment.trkseg->sid==mu2e::SurfaceIdDetail::TT_Front) { return true; }
    else { return false; }
  }
  if (segment.trksegmc != nullptr) {
    if (segment.trksegmc->sid==mu2e::SurfaceIdDetail::TT_Front) { return true; }
    else { return false; }
  }
  else { return false; }
}

bool tracker_middle(TrackSegment& segment) {  // track fit segment is at the middle of the tracker
  if (segment.trkseg != nullptr) {
    if (segment.trkseg->sid==mu2e::SurfaceIdDetail::TT_Mid) { return true; }
    else { return false; }
  }
  if (segment.trksegmc != nullptr) {
    if (segment.trksegmc->sid==mu2e::SurfaceIdDetail::TT_Mid) { return true; }
    else { return false; }
  }
  else { return false; }
}

bool tracker_exit(TrackSegment& segment) { // track fit segment is at the tracker exit
  if (segment.trkseg != nullptr) {
    if (segment.trkseg->sid==mu2e::SurfaceIdDetail::TT_Back) { return true; }
    else { return false; }
  }
  if (segment.trksegmc != nullptr) {
    if (segment.trksegmc->sid==mu2e::SurfaceIdDetail::TT_Back) { return true; }
    else { return false; }
  }
  else { return false; }
}

bool stopping_target_foils(TrackSegment& segment) { // track fit segment is in the stopping target
  if (segment.trkseg != nullptr) {
    if (segment.trkseg->sid==mu2e::SurfaceIdDetail::ST_Foils) { return true; }
    else { return false; }
  }
  else if (segment.trksegmc != nullptr) {
    if (segment.trksegmc->sid==mu2e::SurfaceIdDetail::ST_Foils) { return true; }
    else { return false; }
  }
  else { return false; }
}

bool inner_proton_absorber(TrackSegment& segment) { // track fit segment is in the stopping target
  if (segment.trkseg != nullptr) {
    if (segment.trkseg->sid==mu2e::SurfaceIdDetail::IPA) { return true; }
    else { return false; }
  }
  else if (segment.trksegmc != nullptr) {
    if (segment.trksegmc->sid==mu2e::SurfaceIdDetail::IPA) { return true; }
    else { return false; }
  }
  else { return false; }
}


//+ Track Segment Cuts - Other
bool has_mc_step(TrackSegment& segment) { // track fit segment has an MC-truth SurfaceStep
  if (segment.trksegmc != nullptr) { return true; }
  else { return false; }
}
bool has_reco_step(TrackSegment& segment) { // track fit segment has an reco SurfaceStep
  if (segment.trkseg != nullptr) { return true; }
  else { return false; }
}

//+ Track Cuts - Direction
bool is_downstream(Track& track) { // reconstructed fit segment at tracker middle is going in +z direction
  auto tracker_middle_segments = track.GetSegments([](TrackSegment& segment){ return tracker_middle(segment) && has_reco_step(segment); });
  if (tracker_middle_segments.size() > 1) {
    std::cout << "WARNING: " << tracker_middle_segments.size() << " track segments at tracker middle. Something might have gone wrong..." << std::endl;
    return false;
  }
  else if (tracker_middle_segments.size() == 0) {
    return false;
  }
  return is_downstream(tracker_middle_segments.at(0));
}

bool is_upstream(Track& track) { // reconstructed fit segment at tracker middle is going in -z direction
  auto tracker_middle_segments = track.GetSegments([](TrackSegment& segment){ return tracker_middle(segment) && !has_mc_step(segment) && has_reco_step(segment); });
  if (tracker_middle_segments.size() > 1) {
    std::cout << "WARNING: " << tracker_middle_segments.size() << " track segments at tracker middle. Something might have gone wrong..." << std::endl;
    return false;
  }
  else if (tracker_middle_segments.size() == 0) {
    return false;
  }
  return is_upstream(tracker_middle_segments.at(0));
}

//+ Track Cuts - Particle Types
bool is_particle(Track& track, mu2e::PDGCode::type particle) { // track fit used particle hypothesis
  if (track.trk->pdg == particle) { return true; }
  else { return false; }
}

bool is_e_minus(Track& track) { // track fit used e-minus hypothesis
  return is_particle(track, mu2e::PDGCode::e_minus);
}

bool is_e_plus(Track& track) { // track fit used e-plus hypothesis
  return is_particle(track, mu2e::PDGCode::e_plus);
}

bool is_mu_minus(Track& track) { // track fit used mu-minus hypothesis
  return is_particle(track, mu2e::PDGCode::mu_minus);
}

bool is_mu_plus(Track& track) { // track fit used mu-plus hypothesis
  return is_particle(track, mu2e::PDGCode::mu_plus);
}

//+ Track Cuts - Other
bool is_reflected(Track& track) { // track is refelected (i.e. has both an upstream and downstream fit segment at the tracker entrance)
  bool have_upstream = false;
  bool have_downstream = false;
  for (auto& segment : track.segments) {
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

bool passes_trkqual(Track& track, double cut_val) { // true if trkqual > cut_val
  if (track.trkqual->result > cut_val) { return true; }
  else { return false; }
}

bool passes_trkpid(Track& track, double cut_val) { // true if trkpid > cut_val
  if (track.trkpid->result > cut_val) { return true; }
  else { return false; }
}

//+ CrvCoinc Cuts
bool three_of_four_coinc(CrvCoinc& crv_coinc) { // CRV coincidence has exactly three layers hit
  if (crv_coinc.reco->nLayers == 3) { return true; }
  else { return false; }
}

//+ Combined Track & CrvCoinc Cuts
bool track_crv_coincidence(TrackSegment& segment, CrvCoinc& crv_coinc) {
  if ( abs(segment.trkseg->time - crv_coinc.reco->time) < 150 ) { return true; }
  else { return false; }
}

//+ MCParticle Cuts - Particle Types
bool is_particle(MCParticle& particle, mu2e::PDGCode::type pdg) { // MCParticle has pdg
  if (particle.mcsim->pdg == pdg) { return true; }
  else { return false; }
}
bool is_muon(MCParticle& sim) { // true if the MCParticle is a muon
  return is_particle(sim, mu2e::PDGCode::mu_minus);
}

//+ MCParticle Cuts - Start Process Code
bool start_process(MCParticle& particle, mu2e::ProcessCode::enum_type proc) { // MCParticle has this creation code
  if (particle.mcsim->startCode == proc) { return true; }
  else { return false; }
}
bool is_CeMinusEndpoint(MCParticle& particle) { // true of the MCParticle is CeMinusEndpoint
  return start_process(particle, mu2e::ProcessCode::mu2eCeMinusEndpoint);
}
bool is_CeMinusLeadingLog(MCParticle& particle) { // true of the MCParticle is CeMinusLeadingLog
  return start_process(particle, mu2e::ProcessCode::mu2eCeMinusLeadingLog);
}
bool is_CePlusEndpoint(MCParticle& particle) { // true of the MCParticle is CePlusEndpoint
  return start_process(particle, mu2e::ProcessCode::mu2eCePlusEndpoint);
}
bool is_CePlusLeadingLog(MCParticle& particle) { // true of the MCParticle is CePlusLeadingLog
  return start_process(particle, mu2e::ProcessCode::mu2eCePlusLeadingLog);
}

bool is_DIO(MCParticle& particle) { // true of the MCParticle is DIO or mu2eDIOLeadingLof
  return (start_process(particle, mu2e::ProcessCode::mu2eMuonDecayAtRest) || start_process(particle, mu2e::ProcessCode::DIO));
}

//+ MCParticle Cuts - Stop Process Code
bool end_process(MCParticle& particle, mu2e::ProcessCode::enum_type proc) { // MCParticle has this stop code
  if (particle.mcsim->stopCode == proc) { return true; }
  else { return false; }
}

//+ MCParticle Cuts - Generator ID
bool from_gen_id(MCParticle& particle, mu2e::GenId::enum_type genid) { // MCParticle has this generator ID
  if (particle.mcsim->gen == genid) { return true; }
  else { return false; }
}

bool is_cosmic(MCParticle& particle) { // MCParticle is a cosmic. This uses the function GenId::isCosmic() from Offline
  return mu2e::GenId(mu2e::GenId::enum_type(particle.mcsim->gen)).isCosmic();
}

//+ MCParticle Cuts - Genealogy
bool has_track_relationship(MCParticle& particle, mu2e::MCRelationship::relation rel) { // MCParticle has this relationship to track primary
  if (particle.mcsim->trkrel == rel) { return true; }
  else { return false; }
}
bool is_track_particle(MCParticle& particle) { // MCParticle with the most hits on the track
  return has_track_relationship(particle, mu2e::MCRelationship::same) && particle.mcsim->rank == 0;
}
bool has_hit_on_track(MCParticle& particle) { // MCParticle has a hit on the track
  return has_track_relationship(particle, mu2e::MCRelationship::same);
}
bool is_track_parent(MCParticle& particle) { // MCParticle is the parent of a particle with a hit on thw track
  return has_track_relationship(particle, mu2e::MCRelationship::mother);
}
bool has_primary_relationship(MCParticle& particle, mu2e::MCRelationship::relation rel) { // MCParticle has this relationship to event primary
  if (particle.mcsim->prirel == rel) { return true; }
  else { return false; }
}

//+ MCParticle Cuts - Other
bool made_track_hit(MCParticle& particle) { // MCParticle made a hit in the tracker
  if (particle.mcsim->nhits > 0) { return true; }
  else { return false; }
}

bool from_gen_id(Track& track, mu2e::GenId::enum_type genid) { // Track's MCParticle has this generator ID
  if (track.GetMCParticles(is_track_particle).at(0).mcsim->gen == genid) { return true; }
  else { return false; }
}

bool from_start_process(Track& track, mu2e::ProcessCode::enum_type proc) { // Track's MCParticle has this starting ProcessCode
  if (track.GetMCParticles(is_track_particle).at(0).mcsim->startCode == proc) { return true; }
  else { return false; }
}

//+ Trigger Cut
bool passes_trigger(Event& event, std::string trigname) { // true if the event passed the named trigger
  if (trigname.substr(0, 5) != "trig_") { // we need to add the prefix to get the correct element
    trigname = "trig_" + trigname;
  }
  const auto i_trig_element = event.trigNameMap[trigname];
  return event.triginfo._triggerArray[i_trig_element];
}
#endif
