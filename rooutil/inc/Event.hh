#ifndef Event_hh_
#define Event_hh_

#include <algorithm>
#include <cxxabi.h> // For abi::__cxa_demangle

#include "EventNtuple/inc/EventInfo.hh"
#include "EventNtuple/inc/EventInfoMC.hh"
#include "EventNtuple/inc/HitCount.hh"
#include "EventNtuple/inc/CrvSummaryReco.hh"
#include "EventNtuple/inc/CrvSummaryMC.hh"

#include "EventNtuple/inc/TrkInfoMC.hh"
#include "EventNtuple/inc/TrkCaloHitInfo.hh"
#include "EventNtuple/inc/CaloClusterInfoMC.hh"

#include "EventNtuple/inc/CaloClusterInfo.hh"
#include "EventNtuple/inc/CaloHitInfo.hh"
#include "EventNtuple/inc/CaloRecoDigiInfo.hh"
#include "EventNtuple/inc/CaloDigiInfo.hh"

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
#include "EventNtuple/inc/TrkStrawHitCalibInfo.hh"

#include "EventNtuple/inc/MVAResultInfo.hh"

#include "EventNtuple/inc/TrigInfo.hh"

#include "EventNtuple/rooutil/inc/Track.hh"
#include "EventNtuple/rooutil/inc/CrvCoinc.hh"
#include "EventNtuple/rooutil/inc/CaloCluster.hh"

#include "TChain.h"

struct Event {
  Event(TChain* ntuple) {
    CheckForBranch(ntuple, "evtinfo", &this->evtinfo);
    CheckForBranch(ntuple, "hitcount", &this->hitcount);
    CheckForBranch(ntuple, "crvsummary", &this->crvsummary);
    const auto& branches = ntuple->GetListOfBranches();
    int i_trig_branch = 0;
    for (const auto& branch : *branches) {
      std::string brname = branch->GetName();
      if (brname.substr(0, 5) == "trig_") {
        ntuple->SetBranchAddress(brname.c_str(), &this->triginfo._triggerArray[i_trig_branch]);
        trigNameMap.insert({brname, i_trig_branch});
        i_trig_branch++;
      }
    }

    CheckForBranch(ntuple, "trk", &this->trk);
    CheckForBranch(ntuple, "trksegs", &this->trksegs);
    CheckForBranch(ntuple, "trkcalohit", &this->trkcalohit);
    CheckForBranch(ntuple, "trkqual", &this->trkqual);
    CheckForBranch(ntuple, "trkqual3", &this->trkqual_alt); // TODO: un-hardcode those
    CheckForBranch(ntuple, "crvcoincs", &this->crvcoincs);
    CheckForBranch(ntuple, "trkpid", &this->trkpid);

    // Check if the MC branches exist
    CheckForBranch(ntuple, "evtinfomc", &this->evtinfomc);
    CheckForBranch(ntuple, "crvsummarymc", &this->crvsummarymc);
    CheckForBranch(ntuple, "trkmc", &this->trkmc);
    CheckForBranch(ntuple, "trksegsmc", &this->trksegsmc);
    CheckForBranch(ntuple, "trksegpars_lh", &this->trksegpars_lh);
    CheckForBranch(ntuple, "trksegpars_ch", &this->trksegpars_ch);
    CheckForBranch(ntuple, "trksegpars_kl", &this->trksegpars_kl);
    CheckForBranch(ntuple, "trkcalohitmc", &this->trkcalohitmc);
    CheckForBranch(ntuple, "crvcoincsmc", &this->crvcoincsmc);
    CheckForBranch(ntuple, "crvdigis", &this->crvdigis);
    CheckForBranch(ntuple, "crvpulses", &this->crvpulses);
    CheckForBranch(ntuple, "crvpulsesmc", &this->crvpulsesmc);
    CheckForBranch(ntuple, "crvcoincsmcplane", &this->crvcoincsmcplane);
    
    CheckForBranch(ntuple, "trkmcsim", &this->trkmcsim);
    CheckForBranch(ntuple, "trkhits", &this->trkhits);
    CheckForBranch(ntuple, "trkhitsmc", &this->trkhitsmc);
    CheckForBranch(ntuple, "trkmats", &this->trkmats);
    CheckForBranch(ntuple, "trkhitcalibs", &this->trkhitcalibs);

    CheckForBranch(ntuple, "caloclusters", &this->caloclusters);
    CheckForBranch(ntuple, "calohits", &this->calohits);
    CheckForBranch(ntuple, "calorecodigis", &this->calorecodigis);
    CheckForBranch(ntuple, "calodigis", &this->calodigis);

  }

  // Check if a branch exists in the TChain, and optionally set its address
  bool CheckForBranch(TChain* ntuple, const char* branch_name, void* address = nullptr) {
    if(ntuple->GetBranch(branch_name) == nullptr || ntuple->GetBranchStatus(branch_name) == 0) return false;
    if(address != nullptr) ntuple->SetBranchAddress(branch_name, address);
    return true;
  }

  void Update(bool debug = false) {
    if (debug) { std::cout << "Event::Update(): Clearing previous Tracks... " << std::endl; }
    tracks.clear();
    for (int i_track = 0; i_track < nTracks(); ++i_track) {
      if (debug) { std::cout << "Event::Update(): Creating Track " << i_track << "... " << std::endl; }
      Track track(&(trk->at(i_track)), &(trksegs->at(i_track)), &(trkcalohit->at(i_track))); // passing the addresses of the underlying structs
      UpdateObject(track.trkmc, trkmc, i_track, debug);
      UpdateObject(track.trksegsmc, trksegsmc, i_track, debug);
      UpdateObject(track.trksegpars_lh, trksegpars_lh, i_track, debug);
      UpdateObject(track.trksegpars_ch, trksegpars_ch, i_track, debug);
      UpdateObject(track.trksegpars_kl, trksegpars_kl, i_track, debug);
      UpdateObject(track.trkmcsim, trkmcsim, i_track, debug);
      UpdateObject(track.trkhits, trkhits, i_track, debug);
      UpdateObject(track.trkhitsmc, trkhitsmc, i_track, debug);
      UpdateObject(track.trkmats, trkmats, i_track, debug);
      UpdateObject(track.trkhitcalibs, trkhitcalibs, i_track, debug);
      UpdateObject(track.trkqual, trkqual, i_track, debug);
      UpdateObject(track.trkqual_alt, trkqual_alt, i_track, debug);
      UpdateObject(track.trkpid, trkpid, i_track, debug);

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

    if (caloclusters != nullptr) {
      if (debug) { std::cout << "Event::Update(): Clearing previous CaloClusters... " << std::endl; }
      calo_clusters.clear();
      for (int i_cluster = 0; i_cluster < nCaloClusters(); ++i_cluster) {
        if (debug) { std::cout << "Event::Update(): Creating CaloCluster " << i_cluster << "... " << std::endl; }
        CaloCluster calo_cluster(&(caloclusters->at(i_cluster))); // passing the addresses of the underlying structs
        calo_clusters.emplace_back(calo_cluster);
      }
    }
  }

  template <typename T> void UpdateObject(T*& object, std::vector<T>* object_ptr, int index, bool debug = false) {
    if (object_ptr != nullptr) {
      if (debug) {
        std::cout << "Event::Update(): Adding "
                  << abi::__cxa_demangle(typeid(*object).name(), nullptr, nullptr, nullptr) << " to Track " << index << "... " << std::endl;
      }
      object = &(object_ptr->at(index));
    } else if(debug) {
      std::cout << "Event::Update(): No " << abi::__cxa_demangle(typeid(*object).name(), nullptr, nullptr, nullptr) << " to add to Track " << index << std::endl;
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
  int nCaloClusters() const {
    if (caloclusters == nullptr) { return 0; }
    else { return caloclusters->size(); }
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
        if (trkqual_alt) { trkqual_alt->erase(trkqual_alt->begin()+trks_to_remove[i_trk]); }
        if (trkpid) { trkpid->erase(trkpid->begin()+trks_to_remove[i_trk]); }
        if (trksegpars_lh) { trksegpars_lh->erase(trksegpars_lh->begin()+trks_to_remove[i_trk]); }
        if (trksegpars_ch) { trksegpars_ch->erase(trksegpars_ch->begin()+trks_to_remove[i_trk]); }
        if (trksegpars_kl) { trksegpars_kl->erase(trksegpars_kl->begin()+trks_to_remove[i_trk]); }
        if (trkhits) { trkhits->erase(trkhits->begin()+trks_to_remove[i_trk]); }
        if (trkhitsmc) { trkhitsmc->erase(trkhitsmc->begin()+trks_to_remove[i_trk]); }
        if (trkmats) { trkmats->erase(trkmats->begin()+trks_to_remove[i_trk]); }
        if (trkhitcalibs) { trkhitcalibs->erase(trkhitcalibs->begin()+trks_to_remove[i_trk]); }
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

  CaloClusters GetCaloClusters() { return calo_clusters; }
  CaloClusters GetCaloClusters(CaloClusterCut cut, bool inplace = false) {
    if (!inplace) { // if we are not changing inplace, then just create a new vector to return
      CaloClusters select_calo_clusters;
      for (auto& calo_cluster : calo_clusters) {
        if (cut(calo_cluster)) {
          select_calo_clusters.emplace_back(calo_cluster);
        }
      }
      return select_calo_clusters;
    }
    else {
      auto newEnd = std::remove_if(calo_clusters.begin(), calo_clusters.end(), [cut](CaloCluster& calo_cluster) { return !cut(calo_cluster); });

      std::vector<size_t> caloclusters_to_remove;
      for (std::vector<CaloCluster>::iterator i_calo_cluster = newEnd; i_calo_cluster != calo_clusters.end(); ++i_calo_cluster) { // now need to remove from event
        for (size_t i_calocluster = 0; i_calocluster < caloclusters->size(); ++i_calocluster) {
          if (&(caloclusters->at(i_calocluster))  == i_calo_cluster->calocluster) {
            caloclusters_to_remove.emplace_back(i_calocluster);
            // flag i_calocluster for remoavel
          }
        }
      }
      for (int i_calocluster = caloclusters_to_remove.size()-1; i_calocluster >= 0; --i_calocluster) {
        caloclusters->erase(caloclusters->begin()+caloclusters_to_remove[i_calocluster]);
      }

      calo_clusters.erase(newEnd, calo_clusters.end()); // remove only rearranges and returns the new end
      return calo_clusters;
    }
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

  int CountCaloClusters() { return calo_clusters.size(); }
  int CountCaloClusters(CaloClusterCut cut) {
    CaloClusters select_calo_clusters = GetCaloClusters(cut);
    return select_calo_clusters.size();
  }


  Tracks tracks;
  CrvCoincs crv_coincs;
  CaloClusters calo_clusters;

  // Pointers to the data
  mu2e::EventInfo* evtinfo = nullptr;
  mu2e::EventInfoMC* evtinfomc = nullptr;
  mu2e::HitCount* hitcount = nullptr;
  mu2e::CrvSummaryReco* crvsummary = nullptr;
  mu2e::CrvSummaryMC* crvsummarymc = nullptr;
  mu2e::TrigInfo triginfo; // not a pointer because we give the address of array elements inside this

  std::vector<mu2e::TrkInfo>* trk = nullptr;
  std::vector<mu2e::TrkInfoMC>* trkmc = nullptr;
  std::vector<mu2e::TrkCaloHitInfo>* trkcalohit = nullptr;
  std::vector<mu2e::CaloClusterInfoMC>* trkcalohitmc = nullptr;
  std::vector<mu2e::MVAResultInfo>* trkqual = nullptr;
  std::vector<mu2e::MVAResultInfo>* trkqual_alt = nullptr; // an optional trkqual branch to also use
  std::vector<mu2e::MVAResultInfo>* trkpid = nullptr;
  std::vector<std::vector<mu2e::TrkSegInfo>>* trksegs = nullptr;
  std::vector<std::vector<mu2e::SurfaceStepInfo>>* trksegsmc = nullptr;
  std::vector<std::vector<mu2e::LoopHelixInfo>>* trksegpars_lh = nullptr;
  std::vector<std::vector<mu2e::CentralHelixInfo>>* trksegpars_ch = nullptr;
  std::vector<std::vector<mu2e::KinematicLineInfo>>* trksegpars_kl = nullptr;
  std::vector<std::vector<mu2e::TrkStrawHitInfo>>* trkhits = nullptr;
  std::vector<std::vector<mu2e::TrkStrawHitInfoMC>>* trkhitsmc = nullptr;
  std::vector<std::vector<mu2e::TrkStrawMatInfo>>* trkmats = nullptr;
  std::vector<std::vector<mu2e::TrkStrawHitCalibInfo>>* trkhitcalibs = nullptr;

  std::vector<mu2e::CaloClusterInfo>* caloclusters = nullptr;
  std::vector<mu2e::CaloHitInfo>* calohits = nullptr;
  std::vector<mu2e::CaloRecoDigiInfo>* calorecodigis = nullptr;
  std::vector<mu2e::CaloDigiInfo>* calodigis = nullptr;

  std::vector<mu2e::CrvHitInfoReco>* crvcoincs = nullptr;
  std::vector<mu2e::CrvHitInfoMC>* crvcoincsmc = nullptr;
  std::vector<mu2e::CrvWaveformInfo>* crvdigis = nullptr;
  std::vector<mu2e::CrvPulseInfoReco>* crvpulses = nullptr;
  std::vector<mu2e::CrvHitInfoMC>* crvpulsesmc = nullptr;
  std::vector<mu2e::CrvPlaneInfoMC>* crvcoincsmcplane = nullptr;

  std::vector<std::vector<mu2e::SimInfo>>* trkmcsim = nullptr;

  // Need to keep track of trigger name to element in triginfo
  std::map<std::string, unsigned int> trigNameMap;
};

#endif
