#ifndef Track_hh_
#define Track_hh_

#include <functional>
#include "EventNtuple/inc/TrkInfo.hh"
#include "EventNtuple/inc/TrkSegInfo.hh"
#include "EventNtuple/inc/TrkCaloHitInfo.hh"

#include "EventNtuple/utils/rooutil/inc/TrackSegment.hh"
#include "EventNtuple/utils/rooutil/inc/MCParticle.hh"
#include "EventNtuple/utils/rooutil/inc/TrackHit.hh"

struct Track {
  Track(mu2e::TrkInfo* trk, std::vector<mu2e::TrkSegInfo>* trksegs, mu2e::TrkCaloHitInfo* trkcalohit)
    : trk(trk), trksegs(trksegs), trkcalohit(trkcalohit) {

    // Create the underlying track segments
    for (int i_segment = 0; i_segment < nSegments(); ++i_segment) {
      TrackSegment segment(&(trksegs->at(i_segment))); // passing the addresses of the underlying structs
      segments.emplace_back(segment);
    }
  }

  void Update(bool debug = false) {
    if (trksegsmc != nullptr) { // if we have MC information
      // search for corresponding SurfaceStepInfo (it will have the same sid and sindex)
      // also, trksegs and trksegms may not be the same length...
      if (debug) { std::cout << "Track::Update(): Updating trksegsmc..." << std::endl; }
      for (size_t i_segment_mc = 0; i_segment_mc < trksegsmc->size(); ++i_segment_mc) {
        if (debug) { std::cout << "Track::Update(): Looking for trkseg to match trksegmc " << i_segment_mc << ".(of " << trksegsmc->size() << ").. "; }
        bool reco_step_found = false;
        double min_dt = 999999;
        double max_dt = 5; // want to be at least this close
        for (auto& segment : segments) {
          if (segment.trkseg != nullptr) { // we might have added MC-only segments...

            // Want to to match segments that are at the same surface (+ index) travelling in the same direction and closest in time
            double dt = std::fabs(segment.trkseg->time - trksegsmc->at(i_segment_mc).time);
            if (segment.trkseg->sid == trksegsmc->at(i_segment_mc).sid
                && segment.trkseg->sindex == trksegsmc->at(i_segment_mc).sindex
                && segment.trkseg->mom.Dot(trksegsmc->at(i_segment_mc).mom)>0
                && dt < max_dt) {

              if (dt < min_dt) {
                min_dt = dt;
                segment.trksegmc = &(trksegsmc->at(i_segment_mc));
                reco_step_found = true;
                if (debug) { std::cout << "FOUND (reco step mom = " << segment.trkseg->mom.R() << ", MC step mom = " << segment.trksegmc->mom.R() << ", dt = " << dt << ")" << std::endl; }
              }
            }
          }
        }
        if (!reco_step_found) {
          if (debug) { std::cout << "NOT FOUND. Adding MC-only TrackSegment..." << std::endl; }
          TrackSegment segment_mc;
          segment_mc.trksegmc = &(trksegsmc->at(i_segment_mc));
          segments.emplace_back(segment_mc);
        }
      }
    }

    if (trkmcsim != nullptr) {
      if (debug) { std::cout << "Track::Update(): Updating trkmcsim..." << std::endl; }
      // Create the underlying MCParticles if possible
      for (int i_mc_particle = 0; i_mc_particle < nMCParticles(); ++i_mc_particle) {
        if (debug) { std::cout << "Track::Update(): Creating MCParticle " << i_mc_particle << " / " << nMCParticles() << "..." << std::endl; }
        MCParticle mc_particle(&(trkmcsim->at(i_mc_particle))); // passing the addresses of the underlying structs
        mc_particles.emplace_back(mc_particle);
      }
    }

    if (trksegpars_lh != nullptr) { // if we LoopHelix info
      if (debug) { std::cout << "Track::Update(): Updating trksegpars_lh..." << std::endl; }
      for (int i_segment = 0; i_segment < nSegments(); ++i_segment) {
        segments[i_segment].trksegpars_lh = &(trksegpars_lh->at(i_segment));
      }
    }
    if (trksegpars_ch != nullptr) { // if we CentralHelix info
      if (debug) { std::cout << "Track::Update(): Updating trksegpars_ch..." << std::endl; }
      for (int i_segment = 0; i_segment < nSegments(); ++i_segment) {
        segments[i_segment].trksegpars_ch = &(trksegpars_ch->at(i_segment));
      }
    }
    if (trksegpars_kl != nullptr) { // if we CentralHelix info
      if (debug) { std::cout << "Track::Update(): Updating trksegpars_kl..." << std::endl; }
      for (int i_segment = 0; i_segment < nSegments(); ++i_segment) {
        segments[i_segment].trksegpars_kl = &(trksegpars_kl->at(i_segment));
      }
    }

    if (trkhits != nullptr) {
      if (debug) { std::cout << "Track::Update(): Updating trkhits..." << std::endl; }
      // Create the underlying TrackHits
      for (int i_trkhit = 0; i_trkhit < nHits(); ++i_trkhit) {
        TrackHit trkhit;
        trkhit.reco = &(trkhits->at(i_trkhit)); // passing the addresses of the underlying structs

        // the first trkhitsmc correspond 1:1 with the reco hits, the remaining trkhitsmc also go with the truth
        if (trkhitsmc != nullptr) {
          trkhit.mc = &(trkhitsmc->at(i_trkhit));
        }
        hits.emplace_back(trkhit);
      }
      if (trkhitsmc != nullptr) {
        if (debug) { std::cout << "Track::Update(): Updating trkhitsmc..." << std::endl; }
        // Fill in the remainder of the track hits
        for (size_t i_trkhitmc = nHits(); i_trkhitmc < trkhitsmc->size(); ++i_trkhitmc) {
          TrackHit trkhit;
          trkhit.mc = &(trkhitsmc->at(i_trkhitmc));
          hits.emplace_back(trkhit);
        }
      }
    }
  }

  int nSegments() const { return trksegs->size(); }
  TrackSegments GetSegments() { return segments; }
  TrackSegments GetSegments(TrackSegmentCut cut) {
    TrackSegments select_segments;
    for (auto& segment : segments) {
      if (cut(segment)) {
        select_segments.emplace_back(segment);
      }
    }
    return select_segments;
  }
  TrackSegments segments;

  int nMCParticles() const {
    if (trkmcsim == nullptr) { return 0; }
    else { return trkmcsim->size(); }
  }
  MCParticles GetMCParticles() { return mc_particles; }
  MCParticles GetMCParticles(MCParticleCut cut) {
    MCParticles select_mc_particles;
    for (auto& mc_particle : mc_particles) {
      if (cut(mc_particle)) {
        select_mc_particles.emplace_back(mc_particle);
      }
    }
    return select_mc_particles;
  }
  MCParticles mc_particles;

  int nHits() const { return trkhits->size(); }
  TrackHits GetHits() { return hits; }
  TrackHits GetHits(TrackHitCut cut) {
    TrackHits select_hits;
    for (auto& hit : hits) {
      if (cut(hit)) {
        select_hits.emplace_back(hit);
      }
    }
    return select_hits;
  }
  TrackHits hits;

  // Pointers to the data
  mu2e::TrkInfo* trk = nullptr;
  mu2e::TrkInfoMC* trkmc = nullptr;
  std::vector<mu2e::TrkSegInfo>* trksegs = nullptr;
  std::vector<mu2e::SurfaceStepInfo>* trksegsmc = nullptr;
  std::vector<mu2e::LoopHelixInfo>* trksegpars_lh = nullptr;
  std::vector<mu2e::CentralHelixInfo>* trksegpars_ch = nullptr;
  std::vector<mu2e::KinematicLineInfo>* trksegpars_kl = nullptr;
  std::vector<mu2e::TrkStrawHitInfo>* trkhits = nullptr;
  std::vector<mu2e::TrkStrawHitInfoMC>* trkhitsmc = nullptr;
  std::vector<mu2e::TrkStrawMatInfo>* trkmats = nullptr;
  mu2e::TrkCaloHitInfo* trkcalohit = nullptr;
  std::vector<mu2e::SimInfo>* trkmcsim = nullptr;
  mu2e::MVAResultInfo* trkqual = nullptr;
};

typedef std::function<bool(Track&)> TrackCut;
typedef std::vector<Track> Tracks;

#endif
