#ifndef Event_hh_
#define Event_hh_

#include <algorithm>

#include "EventNtuple/inc/EventInfo.hh"
#include "EventNtuple/inc/EventInfoMC.hh"
#include "EventNtuple/inc/HitCount.hh"
#include "EventNtuple/inc/CrvSummaryReco.hh"
#include "EventNtuple/inc/CrvSummaryMC.hh"

#include "EventNtuple/inc/TrkInfoMC.hh"
#include "EventNtuple/inc/TrkCaloHitInfo.hh"
#include "EventNtuple/inc/CaloClusterInfoMC.hh"

#include "EventNtuple/inc/CrvHitInfoReco.hh"
#include "EventNtuple/inc/CrvHitInfoMC.hh"
#include "EventNtuple/inc/CrvWaveformInfo.hh"
#include "EventNtuple/inc/CrvPulseInfoReco.hh"
#include "EventNtuple/inc/CrvPlaneInfoMC.hh"

#include "EventNtuple/inc/SimInfo.hh"
#include "EventNtuple/inc/LoopHelixInfo.hh"
#include "EventNtuple/inc/CentralHelixInfo.hh"
#include "EventNtuple/inc/KinematicLineInfo.hh"
#include "EventNtuple/inc/TrkStrawHitInfo.hh"
#include "EventNtuple/inc/TrkStrawHitInfoMC.hh"
#include "EventNtuple/inc/TrkStrawMatInfo.hh"

#include "EventNtuple/inc/MVAResultInfo.hh"

#include "EventNtuple/utils/rooutil/inc/Track.hh"
#include "EventNtuple/utils/rooutil/inc/CrvCoinc.hh"

#include "TChain.h"

struct Event {
  Event(TChain* ntuple) {

    if (ntuple->GetBranch("evtinfo")) {
      ntuple->SetBranchAddress("evtinfo", &this->evtinfo);
    }
    if (ntuple->GetBranch("hitcount")) {
      ntuple->SetBranchAddress("hitcount", &this->hitcount);
    }
    if (ntuple->GetBranch("crvsummary")) {
      ntuple->SetBranchAddress("crvsummary", &this->crvsummary);
    }

    if (ntuple->GetBranch("trk")) {
      ntuple->SetBranchAddress("trk", &this->trk);
    }
    if (ntuple->GetBranch("trksegs")) {
      ntuple->SetBranchAddress("trksegs", &this->trksegs);
    }
    if (ntuple->GetBranch("trkcalohit")) {
      ntuple->SetBranchAddress("trkcalohit", &this->trkcalohit);
    }
    if (ntuple->GetBranch("trkqual")) {
      ntuple->SetBranchAddress("trkqual", &this->trkqual);
    }
    if (ntuple->GetBranch("crvcoincs")) {
      ntuple->SetBranchAddress("crvcoincs", &this->crvcoincs);
    }

    // Check if the MC branches exist
    if (ntuple->GetBranch("evtinfomc")) {
      ntuple->SetBranchAddress("evtinfomc", &this->evtinfomc);
    }
    if (ntuple->GetBranch("crvsummarymc")) {
      ntuple->SetBranchAddress("crvsummarymc", &this->crvsummarymc);
    }
    if (ntuple->GetBranch("trkmc")) {
      ntuple->SetBranchAddress("trkmc", &this->trkmc);
    }
    if (ntuple->GetBranch("trksegsmc")) {
      ntuple->SetBranchAddress("trksegsmc", &this->trksegsmc);
    }
    if (ntuple->GetBranch("trksegpars_lh")) {
      ntuple->SetBranchAddress("trksegpars_lh", &this->trksegpars_lh);
    }
    if (ntuple->GetBranch("trksegpars_ch")) {
      ntuple->SetBranchAddress("trksegpars_ch", &this->trksegpars_ch);
    }
    if (ntuple->GetBranch("trksegpars_kl")) {
      ntuple->SetBranchAddress("trksegpars_kl", &this->trksegpars_kl);
    }
    if (ntuple->GetBranch("trkcalohitmc")) {
      ntuple->SetBranchAddress("trkcalohitmc", &this->trkcalohitmc);
    }
    if (ntuple->GetBranch("crvcoincsmc")) {
      ntuple->SetBranchAddress("crvcoincsmc", &this->crvcoincsmc);
    }
    if (ntuple->GetBranch("crvdigis")) {
      ntuple->SetBranchAddress("crvdigis", &this->crvdigis);
    }
    if (ntuple->GetBranch("crvpulses")) {
      ntuple->SetBranchAddress("crvpulses", &this->crvpulses);
    }
    if (ntuple->GetBranch("crvpulsesmc")) {
      ntuple->SetBranchAddress("crvpulsesmc", &this->crvpulsesmc);
    }
    if (ntuple->GetBranch("crvcoincsmcplane")) {
      ntuple->SetBranchAddress("crvcoincsmcplane", &this->crvcoincsmcplane);
    }


    if (ntuple->GetBranch("trkmcsim")) {
      ntuple->SetBranchAddress("trkmcsim", &this->trkmcsim);
    }
    if (ntuple->GetBranch("trkhits")) {
      ntuple->SetBranchAddress("trkhits", &this->trkhits);
    }
    if (ntuple->GetBranch("trkhitsmc")) {
      ntuple->SetBranchAddress("trkhitsmc", &this->trkhitsmc);
    }
    if (ntuple->GetBranch("trkmats")) {
      ntuple->SetBranchAddress("trkmats", &this->trkmats);
    }
  };

  void Update(bool debug = false) {
    if (debug) { std::cout << "Event::Update(): Clearing previous Tracks... " << std::endl; }
    tracks.clear();
    for (int i_track = 0; i_track < nTracks(); ++i_track) {
      if (debug) { std::cout << "Event::Update(): Creating Track " << i_track << "... " << std::endl; }
      Track track(&(trk->at(i_track)), &(trksegs->at(i_track)), &(trkcalohit->at(i_track))); // passing the addresses of the underlying structs
      if (trkmc != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trkmc to Track " << i_track << "... " << std::endl; }
        track.trkmc = &(trkmc->at(i_track));
      }
      if (trksegsmc != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trksegsmc to Track " << i_track << "... " << std::endl; }
        track.trksegsmc = &(trksegsmc->at(i_track));
      }
      if (trksegpars_lh != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trksegpars_lh to Track " << i_track << "... " << std::endl; }
        track.trksegpars_lh = &(trksegpars_lh->at(i_track));
      }
      if (trksegpars_ch != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trksegpars_ch to Track " << i_track << "... " << std::endl; }
        track.trksegpars_ch = &(trksegpars_ch->at(i_track));
      }
      if (trksegpars_kl != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trksegpars_kl to Track " << i_track << "... " << std::endl; }
        track.trksegpars_kl = &(trksegpars_kl->at(i_track));
      }
      if (trkmcsim != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trkmcsim to Track " << i_track << "... (size = " << trkmcsim->at(i_track).size() << ")" << std::endl; }
        track.trkmcsim = &(trkmcsim->at(i_track));
      }
      if (trkhits != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trkhits to Track " << i_track << "... " << std::endl; }
        track.trkhits = &(trkhits->at(i_track));
      }
      if (trkhitsmc != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trkhitsmc to Track " << i_track << "... " << std::endl; }
        track.trkhitsmc = &(trkhitsmc->at(i_track));
      }
      if (trkmats != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trkmats to Track " << i_track << "... " << std::endl; }
        track.trkmats = &(trkmats->at(i_track));
      }
      if (trkqual != nullptr) {
        if (debug) { std::cout << "Event::Update(): Adding trkqual to Track " << i_track << "... " << std::endl; }
        track.trkqual = &(trkqual->at(i_track));
      }

      if (debug) { std::cout << "Event::Update(): Updating Track " << i_track << "... " << std::endl; }
      track.Update(debug);
      if (debug) { std::cout << "Event::Update(): Adding Track " << i_track << " to Tracks... " << std::endl; }
      tracks.emplace_back(track);
    }

    if (debug) { std::cout << "Event::Update(): Clearing previous CrvCoincs... " << std::endl; }
    crv_coincs.clear();
    for (int i_crv_coinc = 0; i_crv_coinc < nCrvCoincs(); ++i_crv_coinc) {
      CrvCoinc crv_coinc(&(crvcoincs->at(i_crv_coinc)));
      if (crvcoincsmc != nullptr) {
        crv_coinc.mc = &(crvcoincsmc->at(i_crv_coinc));
      }
      crv_coincs.emplace_back(crv_coinc);
    }
  }

  int nTracks() const {
    if (trk == nullptr) { return 0; }
    else { return trk->size(); }
  }
  int nCrvCoincs() const {
    if (crvcoincs == nullptr) { return 0; }
    else { return crvcoincs->size(); }
  }

  Tracks GetTracks() { return tracks; }
  Tracks GetTracks(TrackCut cut, bool inplace = false) {
    if (!inplace) { // if we are not changing inplace, then just create a new vector to return
      Tracks select_tracks;
      for (auto& track : tracks) {
        if (cut(track)) {
          select_tracks.emplace_back(track);
        }
      }
      return select_tracks;
    }
    else {
      auto newEnd = std::remove_if(tracks.begin(), tracks.end(), [cut](Track& track) { return !cut(track); });

      std::vector<size_t> trks_to_remove;
      for (std::vector<Track>::iterator i_track = newEnd; i_track != tracks.end(); ++i_track) { // now need to remove from event
        for (size_t i_trk = 0; i_trk < trk->size(); ++i_trk) {
          if (&(trk->at(i_trk))  == i_track->trk) {
            trks_to_remove.emplace_back(i_trk);
            // flag i_trk for remoavel
          }
        }
      }
      for (int i_trk = trks_to_remove.size()-1; i_trk >= 0; --i_trk) {
        trk->erase(trk->begin()+trks_to_remove[i_trk]);
        if (trkmc) { trkmc->erase(trkmc->begin()+trks_to_remove[i_trk]); }
        if (trksegs) { trksegs->erase(trksegs->begin()+trks_to_remove[i_trk]); }
        if (trksegsmc) { trksegsmc->erase(trksegsmc->begin()+trks_to_remove[i_trk]); }
        if (trkcalohit) { trkcalohit->erase(trkcalohit->begin()+trks_to_remove[i_trk]); }
        if (trkqual) { trkqual->erase(trkqual->begin()+trks_to_remove[i_trk]); }
        if (trksegpars_lh) { trksegpars_lh->erase(trksegpars_lh->begin()+trks_to_remove[i_trk]); }
        if (trksegpars_ch) { trksegpars_ch->erase(trksegpars_ch->begin()+trks_to_remove[i_trk]); }
        if (trksegpars_kl) { trksegpars_kl->erase(trksegpars_kl->begin()+trks_to_remove[i_trk]); }
        if (trkhits) { trkhits->erase(trkhits->begin()+trks_to_remove[i_trk]); }
        if (trkhitsmc) { trkhitsmc->erase(trkhitsmc->begin()+trks_to_remove[i_trk]); }
        if (trkmats) { trkmats->erase(trkmats->begin()+trks_to_remove[i_trk]); }
      }

      tracks.erase(newEnd, tracks.end()); // remove only rearranges and returns the new end
      return tracks;
    }
  }

  CrvCoincs GetCrvCoincs() { return crv_coincs; }
  CrvCoincs GetCrvCoincs(CrvCoincCut cut) {
    CrvCoincs select_crv_coincs;
    for (auto& crv_coinc : crv_coincs) {
      if (cut(crv_coinc)) {
        select_crv_coincs.emplace_back(crv_coinc);
      }
    }
    return select_crv_coincs;
  }

  int CountTracks() { return tracks.size(); }
  int CountTracks(TrackCut cut) {
    Tracks select_tracks = GetTracks(cut);
    return select_tracks.size();
  }

  void SelectTracks(TrackCut cut) { // will reduce the tracks stored in the event
    GetTracks(cut, true); // change in place
  }

  int CountCrvCoincs() { return crv_coincs.size(); }
  int CountCrvCoincs(CrvCoincCut cut) {
    CrvCoincs select_crv_coincs = GetCrvCoincs(cut);
    return select_crv_coincs.size();
  }

  Tracks tracks;
  CrvCoincs crv_coincs;

  // Pointers to the data
  mu2e::EventInfo* evtinfo = nullptr;
  mu2e::EventInfoMC* evtinfomc = nullptr;
  mu2e::HitCount* hitcount = nullptr;
  mu2e::CrvSummaryReco* crvsummary = nullptr;
  mu2e::CrvSummaryMC* crvsummarymc = nullptr;

  std::vector<mu2e::TrkInfo>* trk = nullptr;
  std::vector<mu2e::TrkInfoMC>* trkmc = nullptr;
  std::vector<mu2e::TrkCaloHitInfo>* trkcalohit = nullptr;
  std::vector<mu2e::CaloClusterInfoMC>* trkcalohitmc = nullptr;
  std::vector<mu2e::MVAResultInfo>* trkqual = nullptr;
  std::vector<std::vector<mu2e::TrkSegInfo>>* trksegs = nullptr;
  std::vector<std::vector<mu2e::SurfaceStepInfo>>* trksegsmc = nullptr;
  std::vector<std::vector<mu2e::LoopHelixInfo>>* trksegpars_lh = nullptr;
  std::vector<std::vector<mu2e::CentralHelixInfo>>* trksegpars_ch = nullptr;
  std::vector<std::vector<mu2e::KinematicLineInfo>>* trksegpars_kl = nullptr;
  std::vector<std::vector<mu2e::TrkStrawHitInfo>>* trkhits = nullptr;
  std::vector<std::vector<mu2e::TrkStrawHitInfoMC>>* trkhitsmc = nullptr;
  std::vector<std::vector<mu2e::TrkStrawMatInfo>>* trkmats = nullptr;

  std::vector<mu2e::CrvHitInfoReco>* crvcoincs = nullptr;
  std::vector<mu2e::CrvHitInfoMC>* crvcoincsmc = nullptr;
  std::vector<mu2e::CrvWaveformInfo>* crvdigis = nullptr;
  std::vector<mu2e::CrvPulseInfoReco>* crvpulses = nullptr;
  std::vector<mu2e::CrvHitInfoMC>* crvpulsesmc = nullptr;
  std::vector<mu2e::CrvPlaneInfoMC>* crvcoincsmcplane = nullptr;

  std::vector<std::vector<mu2e::SimInfo>>* trkmcsim = nullptr;
};

#endif
