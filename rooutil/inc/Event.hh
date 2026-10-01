#ifndef Event_hh_
#define Event_hh_

#include <algorithm>
#include <map>
#include <cxxabi.h> // For abi::__cxa_demangle

#include "EventNtuple/inc/EventInfo.hh"
#include "EventNtuple/inc/EventInfoMC.hh"
#include "EventNtuple/inc/HitCount.hh"
#include "EventNtuple/inc/CrvSummaryReco.hh"
#include "EventNtuple/inc/CrvSummaryMC.hh"

#include "EventNtuple/inc/TrkInfoMC.hh"
#include "EventNtuple/inc/TrkCaloHitInfo.hh"
#include "EventNtuple/inc/CaloClusterInfoMC.hh"
#include "EventNtuple/inc/CaloHitInfoMC.hh"

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
#include <stdexcept>

#include "EventNtuple/inc/TrigInfo.hh"
#include "EventNtuple/inc/MCStepInfo.hh"

#include "EventNtuple/rooutil/inc/Track.hh"
#include "EventNtuple/rooutil/inc/UserBranch.hh"
#include "EventNtuple/rooutil/inc/TimeCluster.hh"
#include "EventNtuple/rooutil/inc/LineSeed.hh"
#include "EventNtuple/rooutil/inc/CrvCoinc.hh"
#include "EventNtuple/rooutil/inc/CaloCluster.hh"
#include "EventNtuple/rooutil/inc/Trigger.hh"
#include "EventNtuple/rooutil/inc/CaloHit.hh"
#include "EventNtuple/rooutil/inc/BranchUtils.hh"

namespace rooutil {
  struct Event {
    // A set of "<collection>hits" branches, keyed by the collection name they belong to
    using HitBranches = std::map<std::string, std::vector<std::vector<mu2e::EventNtupleComboHitInfo>>*>;

    Event(TChain* ntuple) {
      CheckForBranch(ntuple, "evtinfo", &this->evtinfo);
      CheckForBranch(ntuple, "hitcount", &this->hitcount);
      CheckForBranch(ntuple, "crvsummary", &this->crvsummary);
      AddTriggerInfo(ntuple);

      CheckForBranch(ntuple, "trk", &this->trk);
      CheckForBranch(ntuple, "trkdtdt", &this->trkdtdt);
      CheckForBranch(ntuple, "trksegs", &this->trksegs);
      CheckForBranch(ntuple, "trkcalohit", &this->trkcalohit);
      CheckForBranch(ntuple, "trkqual", &this->trkqual);
      CheckForBranch(ntuple, "crvcoincs", &this->crvcoincs);
      CheckForBranch(ntuple, "trkpid", &this->trkpid);

      // Check if the MC branches exist
      CheckForBranch(ntuple, "evtinfomc", &this->evtinfomc);
      CheckForBranch(ntuple, "primary", &this->primary);
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

      CheckForBranch(ntuple, "timeclusters", &this->timeclusters);
      CheckForBranch(ntuple, "lineseeds", &this->lineseeds);

      // Discover every time cluster / line seed collection by branch class, however it is named
      // (Run1B ships several: "timeclusters", "protontimeclusters", "tztimeclusters", ...). The
      // conventional "timeclusters"/"lineseeds" branches above already own a dedicated pointer
      // member; reserve their map slot here too so they show up in *CollectionNames(), and alias
      // them into the map (rather than re-binding) in Update().
      for (const auto& name : FindBranchesOfClass(ntuple, "vector<mu2e::EventNtupleTimeClusterInfo>")) {
        timecluster_branches[name] = nullptr; // reserve a stable map slot before taking its address
        if (name != "timeclusters") {
          ntuple->SetBranchAddress(name.c_str(), &timecluster_branches[name]);
        }
      }
      for (const auto& name : FindBranchesOfClass(ntuple, "vector<mu2e::LineSeedInfo>")) {
        lineseed_branches[name] = nullptr;
        if (name != "lineseeds") {
          ntuple->SetBranchAddress(name.c_str(), &lineseed_branches[name]);
        }
      }
      CheckForBranch(ntuple, "timeclustershits", &this->timeclustershits);

      // Each time cluster / line seed collection can have a companion "<name>hits" branch holding
      // the combo hits of each entry, written when EventNtupleMaker's timeclusters.fillHitsFor /
      // lineseeds.fillHitsFor lists that collection (e.g. "protontimeclustershits",
      // "lineseedshits"). Look one up per discovered collection and key it by the collection name.
      // "timeclustershits" already owns the dedicated pointer member above, so only reserve its map
      // slot here and alias it in BuildTimeClusters(), as is done for the collections themselves.
      for (const auto& entry : timecluster_branches) {
        AddHitBranch(ntuple, entry.first, timecluster_hit_branches, entry.first != "timeclusters");
      }
      for (const auto& entry : lineseed_branches) {
        AddHitBranch(ntuple, entry.first, lineseed_hit_branches, true);
      }
      // alias "timeclusters" straight away rather than waiting for the first BuildTimeClusters(),
      // so that a caller setting up an output ntuple before reading an event still sees it
      if (timecluster_hit_branches.count("timeclusters") != 0) {
        timecluster_hit_branches["timeclusters"] = timeclustershits;
      }

      CheckForBranch(ntuple, "caloclusters", &this->caloclusters);
      CheckForBranch(ntuple, "calohits", &this->calohits);
      CheckForBranch(ntuple, "calorecodigis", &this->calorecodigis);
      CheckForBranch(ntuple, "calodigis", &this->calodigis);
      CheckForBranch(ntuple, "caloclustersmc", &this->caloclustersmc);
      CheckForBranch(ntuple, "calohitsmc", &this->calohitsmc);
      CheckForBranch(ntuple, "calomcsim", &this->calomcsim);

      CheckForBranch(ntuple, "mcsteps_virtualdetector", &this->mcsteps_virtualdetector);
    }

    // Bind the "<collection>hits" branch, if the ntuple has one, into hit_branches[collection].
    // bind == false reserves the map slot without taking the branch over, for a collection whose
    // hits already have a dedicated pointer member (only "timeclusters" does).
    void AddHitBranch(TChain* ntuple, const std::string& collection, HitBranches& hit_branches, bool bind) {
      const std::string hits_name = collection + "hits";
      if (!CheckForBranch(ntuple, hits_name.c_str())) { return; }
      hit_branches[collection] = nullptr; // reserve a stable map slot before taking its address
      if (bind) { ntuple->SetBranchAddress(hits_name.c_str(), &hit_branches[collection]); }
    }

    void SetUserBranches(const std::vector<std::shared_ptr<UserBranchBase>>& branches) {
      user_branches = branches;
    }

    // Add trigger branches and store the path name information
    void AddTriggerInfo(TChain* ntuple) {
      trigger.SetTrigInfo(&triginfo); // pointer to the underlying trigger data read in event-by-event

      // trigger branches are named "trig_<trigger path name>" --> look for these
      const auto& branches = ntuple->GetListOfBranches();
      int i_trig_branch = 0;
      for (const auto& branch : *branches) {
        std::string brname = branch->GetName();
        if (brname.substr(0, 5) == "trig_") {
          ntuple->SetBranchAddress(brname.c_str(), &this->triginfo._triggerArray[i_trig_branch]);
          const std::string trigname = brname.substr(5); // name of the trigger path
          trigger.AssignIndex(i_trig_branch, trigname); // map the array index to the path name
          i_trig_branch++;
        }
      }
    }

    void Update(bool debug = false) {
      if (debug) { std::cout << "Event::Update(): Clearing previous Tracks... " << std::endl; }
      tracks.clear();
      for (int i_track = 0; i_track < nTracks(); ++i_track) {
        if (debug) { std::cout << "Event::Update(): Creating Track " << i_track << "... " << std::endl; }
        Track track(&(trk->at(i_track)), &(trksegs->at(i_track)), &(trkcalohit->at(i_track))); // passing the addresses of the underlying structs
        UpdateObject(track.trkmc, trkmc, i_track, debug);
        UpdateObject(track.trkdtdt, trkdtdt, i_track, debug);
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
        UpdateObject(track.trkpid, trkpid, i_track, debug);
        for (const auto& user_branch : user_branches) {
          if (user_branch->is_bound() && user_branch->scope() == UserBranchScope::Track) {
            track.SetUserBranch(user_branch->name(), user_branch->TrackElementPtr(i_track));
          }
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
          if (i_crv_coinc >= static_cast<int>(crvcoincsmc->size()))
            throw std::runtime_error("CRV coincidence cluster count mismatch between reco and MC collections");
          crv_coinc.mc = &(crvcoincsmc->at(i_crv_coinc));
        }
        crv_coincs.emplace_back(crv_coinc);
      }

      BuildTimeClusters(debug);
      BuildLineSeeds(debug);

      if (caloclusters != nullptr) {
        if (debug) { std::cout << "Event::Update(): Clearing previous CaloClusters... " << std::endl; }
        calo_clusters.clear();
        for (int i_cluster = 0; i_cluster < nCaloClusters(); ++i_cluster) {
          if (debug) { std::cout << "Event::Update(): Creating CaloCluster " << i_cluster << "... " << std::endl; }
          CaloCluster calo_cluster(&(caloclusters->at(i_cluster))); // passing the addresses of the underlying structs
          UpdateObject(calo_cluster.caloclustermc, caloclustersmc, i_cluster, debug); // caloclustersmc has 1:1 element-wise matching with caloclusters branch

          // Because calohits branch contains all calorimeter hits and not just those associated with the cluster
          // we need to loop through and add the correct ones to the CaloCluster class here
          if (calohits != nullptr) {
            for (const auto& calohit_Idx : calo_cluster.calocluster->hits_) { // the indexes into the calohits branch
              CaloHit calohit;
              calohit.reco = &(calohits->at(calohit_Idx)); // passing the addresses of the underlying structs

              // WARNING: calohitsmc is NOT index-aligned with calohits (different art products).
              // Do not use calohitsmc->at(calohit_Idx); look up via caloHitIdx_ instead.
              if (calohitsmc != nullptr) {
                for (int i_mc = 0; i_mc < static_cast<int>(calohitsmc->size()); ++i_mc) {
                  if (calohitsmc->at(i_mc).caloHitIdx_ == static_cast<int>(calohit_Idx)) {
                    calohit.mc = &(calohitsmc->at(i_mc));
                    break;
                  }
                }
              }

              calo_cluster.hits.emplace_back(calohit);
            }
          }

          // Because calomcsim branch contains all calorimeter mc particles and not just those associated with the cluster
          // we need to loop through and add the correct ones to the CaloCluster class here
          if (calomcsim != nullptr) {
            for (auto& calosim_Id : calo_cluster.caloclustermc->simParticleIds) { // the SimParticle IDs
              MCParticle mc_particle;

              // Loop through the calomcsim and look for matching SimParticle IDs
              // TODO: see if we can store simParticle_Idx in the caloclustersmc branch
              for (auto& i_calomcsim : *calomcsim) {
                if (i_calomcsim.id == calosim_Id) {
                  mc_particle.mcsim = &(i_calomcsim); // passing the addresses of the underlying structs
                  break;
                }
              }

              calo_cluster.mc_particles.emplace_back(mc_particle);
            }
          }

          calo_clusters.emplace_back(calo_cluster);
        }
      }
    }

    template <typename T> void UpdateObject(T*& object, std::vector<T>* object_ptr, int index, bool debug = false) {
      if (object_ptr != nullptr) {
        if (debug) {
          std::cout << "Event::Update(): Adding "
                    << abi::__cxa_demangle(typeid(*object).name(), nullptr, nullptr, nullptr) << " to Object " << index << "... " << std::endl;
        }
        object = &(object_ptr->at(index));
      } else if(debug) {
        std::cout << "Event::Update(): No " << abi::__cxa_demangle(typeid(*object).name(), nullptr, nullptr, nullptr) << " to add to Object " << index << std::endl;
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
    int nTimeClusters() const {
      if (timeclusters == nullptr) { return 0; }
      else { return timeclusters->size(); }
    }
    int nLineSeeds() const {
      if (lineseeds == nullptr) { return 0; }
      else { return lineseeds->size(); }
    }
    int nCaloClusters() const {
      if (caloclusters == nullptr) { return 0; }
      else { return caloclusters->size(); }
    }


    const Tracks& GetTracks() { return tracks; }
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
        // Evaluate the cut once per track; wrappers are index-aligned with the backing branch vectors.
        // (std::remove_if can't be used: it leaves the tail in an unspecified state, so the tail is not the rejected tracks)
        std::vector<size_t> trks_to_remove;
        for (size_t i_track = 0; i_track < tracks.size(); ++i_track) {
          if (!cut(tracks[i_track])) { trks_to_remove.emplace_back(i_track); }
        }
        // trkcalohitmc only has entries for tracks with a calo hit, so it is index-aligned with trk only when the sizes match
        const bool trkcalohitmc_aligned = trkcalohitmc && trkcalohitmc->size() == trk->size();
        for (int i_trk = trks_to_remove.size()-1; i_trk >= 0; --i_trk) {
          trk->erase(trk->begin()+trks_to_remove[i_trk]);
          if (trkmc) { trkmc->erase(trkmc->begin()+trks_to_remove[i_trk]); }
          if (trkdtdt) { trkdtdt->erase(trkdtdt->begin()+trks_to_remove[i_trk]); }
          if (trksegs) { trksegs->erase(trksegs->begin()+trks_to_remove[i_trk]); }
          if (trksegsmc) { trksegsmc->erase(trksegsmc->begin()+trks_to_remove[i_trk]); }
          if (trkcalohit) { trkcalohit->erase(trkcalohit->begin()+trks_to_remove[i_trk]); }
          if (trkqual) { trkqual->erase(trkqual->begin()+trks_to_remove[i_trk]); }
          if (trkpid) { trkpid->erase(trkpid->begin()+trks_to_remove[i_trk]); }
          for (const auto& user_branch : user_branches) {
            if (user_branch->is_bound() && user_branch->scope() == UserBranchScope::Track) {
              user_branch->EraseTrack(trks_to_remove[i_trk]);
            }
          }
          if (trksegpars_lh) { trksegpars_lh->erase(trksegpars_lh->begin()+trks_to_remove[i_trk]); }
          if (trksegpars_ch) { trksegpars_ch->erase(trksegpars_ch->begin()+trks_to_remove[i_trk]); }
          if (trksegpars_kl) { trksegpars_kl->erase(trksegpars_kl->begin()+trks_to_remove[i_trk]); }
          if (trkhits) { trkhits->erase(trkhits->begin()+trks_to_remove[i_trk]); }
          if (trkhitsmc) { trkhitsmc->erase(trkhitsmc->begin()+trks_to_remove[i_trk]); }
          if (trkmats) { trkmats->erase(trkmats->begin()+trks_to_remove[i_trk]); }
          if (trkhitcalibs) { trkhitcalibs->erase(trkhitcalibs->begin()+trks_to_remove[i_trk]); }
          if (trkmcsim) { trkmcsim->erase(trkmcsim->begin()+trks_to_remove[i_trk]); }
          if (trkcalohitmc_aligned) { trkcalohitmc->erase(trkcalohitmc->begin()+trks_to_remove[i_trk]); }
        }

        Update(); // erasing shifted the backing vectors, so rebuild the wrappers
        return tracks;
      }
    }

    const CrvCoincs& GetCrvCoincs() { return crv_coincs; }
    CrvCoincs GetCrvCoincs(CrvCoincCut cut) {
      CrvCoincs select_crv_coincs;
      for (auto& crv_coinc : crv_coincs) {
        if (cut(crv_coinc)) {
          select_crv_coincs.emplace_back(crv_coinc);
        }
      }
      return select_crv_coincs;
    }

    //-------------------------------------------------
    // (Re)build the TimeCluster/LineSeed wrappers from the branch vectors they point into. Called
    // by Update() for every new event, and again after an in-place selection: erasing from a
    // backing vector invalidates the pointer held by every wrapper into it, including the copies
    // kept in the named collections, so the wrappers always have to be rebuilt from scratch.

    // The "<collection>hits" branch of a collection, or nullptr if the ntuple does not have one
    std::vector<std::vector<mu2e::EventNtupleComboHitInfo>>* GetHitBranch(const HitBranches& hit_branches,
                                                                         const std::string& collection) const {
      const auto found = hit_branches.find(collection);
      return (found != hit_branches.end()) ? found->second : nullptr;
    }

    void BuildTimeClusters(bool debug = false) {
      if (debug) { std::cout << "Event::BuildTimeClusters(): Clearing previous TimeClusters... " << std::endl; }
      time_clusters.clear();
      if (timeclusters != nullptr) {
        // alias the conventional branches into the maps (their pointers are already refreshed by
        // ROOT via SetBranchAddress on this->timeclusters/this->timeclustershits)
        timecluster_branches["timeclusters"] = timeclusters;
        if (timecluster_hit_branches.count("timeclusters") != 0) {
          timecluster_hit_branches["timeclusters"] = timeclustershits;
        }
      }
      for (auto& entry : timecluster_branches) {
        auto& wrapped = named_time_clusters[entry.first];
        wrapped.clear();
        if (entry.second == nullptr) { continue; }
        auto* hits = GetHitBranch(timecluster_hit_branches, entry.first);
        if (hits != nullptr && hits->size() != entry.second->size()) {
          throw std::runtime_error("Time cluster count mismatch between the " + entry.first + " and " + entry.first + "hits collections");
        }
        for (size_t i_cluster = 0; i_cluster < entry.second->size(); ++i_cluster) {
          if (debug) { std::cout << "Event::BuildTimeClusters(): Creating " << entry.first << " TimeCluster " << i_cluster << "... " << std::endl; }
          TimeCluster time_cluster(&(entry.second->at(i_cluster))); // passing the addresses of the underlying structs
          if (hits != nullptr) { time_cluster.hits = &(hits->at(i_cluster)); }
          wrapped.emplace_back(time_cluster);
        }
      }
      if (timeclusters != nullptr) { time_clusters = named_time_clusters["timeclusters"]; }
    }

    void BuildLineSeeds(bool debug = false) {
      if (debug) { std::cout << "Event::BuildLineSeeds(): Clearing previous LineSeeds... " << std::endl; }
      line_seeds.clear();
      if (lineseeds != nullptr) { lineseed_branches["lineseeds"] = lineseeds; } // see BuildTimeClusters()
      for (auto& entry : lineseed_branches) {
        auto& wrapped = named_line_seeds[entry.first];
        wrapped.clear();
        if (entry.second == nullptr) { continue; }
        auto* hits = GetHitBranch(lineseed_hit_branches, entry.first);
        if (hits != nullptr && hits->size() != entry.second->size()) {
          throw std::runtime_error("Line seed count mismatch between the " + entry.first + " and " + entry.first + "hits collections");
        }
        for (size_t i_seed = 0; i_seed < entry.second->size(); ++i_seed) {
          if (debug) { std::cout << "Event::BuildLineSeeds(): Creating " << entry.first << " LineSeed " << i_seed << "... " << std::endl; }
          LineSeed line_seed(&(entry.second->at(i_seed))); // passing the addresses of the underlying structs
          if (hits != nullptr) { line_seed.hits = &(hits->at(i_seed)); }
          wrapped.emplace_back(line_seed);
        }
      }
      if (lineseeds != nullptr) { line_seeds = named_line_seeds["lineseeds"]; }
    }

    const TimeClusters& GetTimeClusters() { return time_clusters; }
    TimeClusters GetTimeClusters(TimeClusterCut cut, bool inplace = false) {
      if (!inplace) { // if we are not changing inplace, then just create a new vector to return
        TimeClusters select_time_clusters;
        for (auto& time_cluster : time_clusters) {
          if (cut(time_cluster)) {
            select_time_clusters.emplace_back(time_cluster);
          }
        }
        return select_time_clusters;
      }
      else {
        if (timeclusters == nullptr) { return time_clusters; } // nothing to select from
        // Work out which backing entries to keep *before* touching the wrappers: std::remove_if
        // moves the retained wrappers into the tail it leaves behind, so the tail cannot be used to
        // identify the rejected entries (for [reject, keep] the tail holds the *kept* one).
        std::vector<bool> keep_cluster(timeclusters->size(), false);
        for (auto& time_cluster : time_clusters) {
          if (!cut(time_cluster)) { continue; }
          for (size_t i_cluster = 0; i_cluster < timeclusters->size(); ++i_cluster) {
            if (&(timeclusters->at(i_cluster)) == time_cluster.timecluster) {
              keep_cluster[i_cluster] = true; // flag i_cluster for keeping
              break;
            }
          }
        }
        // erase back-to-front so the surviving indices (and the addresses the flags were taken at) stay valid
        auto* hits = GetHitBranch(timecluster_hit_branches, "timeclusters"); // == timeclustershits
        for (int i_cluster = static_cast<int>(timeclusters->size())-1; i_cluster >= 0; --i_cluster) {
          if (keep_cluster[i_cluster]) { continue; }
          timeclusters->erase(timeclusters->begin()+i_cluster);
          if (hits != nullptr) { hits->erase(hits->begin()+i_cluster); } // keep the hit lists index-aligned
        }

        BuildTimeClusters(); // erasing invalidated every wrapper's pointer --> rebuild them all
        return time_clusters;
      }
    }

    const LineSeeds& GetLineSeeds() { return line_seeds; }
    LineSeeds GetLineSeeds(LineSeedCut cut, bool inplace = false) {
      if (!inplace) { // if we are not changing inplace, then just create a new vector to return
        LineSeeds select_line_seeds;
        for (auto& line_seed : line_seeds) {
          if (cut(line_seed)) {
            select_line_seeds.emplace_back(line_seed);
          }
        }
        return select_line_seeds;
      }
      else {
        if (lineseeds == nullptr) { return line_seeds; } // nothing to select from
        // See GetTimeClusters() above for why the rejected entries are identified before the
        // wrappers are touched, rather than from the tail left by std::remove_if.
        std::vector<bool> keep_seed(lineseeds->size(), false);
        for (auto& line_seed : line_seeds) {
          if (!cut(line_seed)) { continue; }
          for (size_t i_seed = 0; i_seed < lineseeds->size(); ++i_seed) {
            if (&(lineseeds->at(i_seed)) == line_seed.lineseed) {
              keep_seed[i_seed] = true; // flag i_seed for keeping
              break;
            }
          }
        }
        auto* hits = GetHitBranch(lineseed_hit_branches, "lineseeds");
        for (int i_seed = static_cast<int>(lineseeds->size())-1; i_seed >= 0; --i_seed) {
          if (keep_seed[i_seed]) { continue; }
          lineseeds->erase(lineseeds->begin()+i_seed);
          if (hits != nullptr) { hits->erase(hits->begin()+i_seed); } // keep the hit lists index-aligned
        }

        BuildLineSeeds(); // erasing invalidated every wrapper's pointer --> rebuild them all
        return line_seeds;
      }
    }

    const CaloClusters& GetCaloClusters() const { return calo_clusters; }
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
        // Evaluate the cut once per cluster (see GetTracks for why std::remove_if is not used)
        std::vector<size_t> caloclusters_to_remove;
        for (size_t i_calocluster = 0; i_calocluster < calo_clusters.size(); ++i_calocluster) {
          if (!cut(calo_clusters[i_calocluster])) { caloclusters_to_remove.emplace_back(i_calocluster); }
        }
        for (int i_calocluster = caloclusters_to_remove.size()-1; i_calocluster >= 0; --i_calocluster) {
          caloclusters->erase(caloclusters->begin()+caloclusters_to_remove[i_calocluster]);
          if (caloclustersmc) { caloclustersmc->erase(caloclustersmc->begin()+caloclusters_to_remove[i_calocluster]); }
        }

        Update(); // erasing shifted the backing vector, so rebuild the wrappers
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

    int CountLineSeeds() { return line_seeds.size(); }
    int CountLineSeeds(LineSeedCut cut) {
      LineSeeds select_line_seeds = GetLineSeeds(cut);
      return select_line_seeds.size();
    }

    //-------------------------------------------------
    // Named time cluster / line seed collections (discovered automatically by branch class; see
    // the constructor). The conventional "timeclusters"/"lineseeds" branches are available both
    // through these by-name accessors and through the dedicated members/accessors above.

    std::vector<std::string> TimeClusterCollectionNames() const {
      // Read from timecluster_branches (populated in the constructor), not named_time_clusters
      // (only populated once Update() has run), so collection names are available immediately.
      std::vector<std::string> names;
      for (const auto& entry : timecluster_branches) { names.push_back(entry.first); }
      return names;
    }
    bool HasTimeClusters(const std::string& name) const {
      // Read from the constructor-populated map so availability can be checked before the first
      // Update() (e.g. right after opening the file, to decide what to book histograms for).
      return timecluster_branches.find(name) != timecluster_branches.end();
    }
    // Whether this collection's combo hit lists were written, i.e. whether the TimeClusters it
    // returns carry hits (see TimeCluster::HasHits()/Hits())
    bool HasTimeClusterHits(const std::string& name) const {
      return timecluster_hit_branches.find(name) != timecluster_hit_branches.end();
    }
    const TimeClusters& GetTimeClusters(const std::string& name) const {
      static const TimeClusters empty;
      const auto found = named_time_clusters.find(name);
      return (found != named_time_clusters.end()) ? found->second : empty;
    }

    std::vector<std::string> LineSeedCollectionNames() const {
      // See TimeClusterCollectionNames() above -- read from the constructor-populated map.
      std::vector<std::string> names;
      for (const auto& entry : lineseed_branches) { names.push_back(entry.first); }
      return names;
    }
    bool HasLineSeeds(const std::string& name) const {
      // See HasTimeClusters() above.
      return lineseed_branches.find(name) != lineseed_branches.end();
    }
    // See HasTimeClusterHits() above.
    bool HasLineSeedHits(const std::string& name) const {
      return lineseed_hit_branches.find(name) != lineseed_hit_branches.end();
    }
    const LineSeeds& GetLineSeeds(const std::string& name) const {
      static const LineSeeds empty;
      const auto found = named_line_seeds.find(name);
      return (found != named_line_seeds.end()) ? found->second : empty;
    }


    Tracks tracks;
    CrvCoincs crv_coincs;
    CaloClusters calo_clusters;
    TimeClusters time_clusters;
    LineSeeds line_seeds;

    // Pointers to the data
    mu2e::EventInfo* evtinfo = nullptr;
    mu2e::EventInfoMC* evtinfomc = nullptr;
    std::vector<mu2e::SimInfo>* primary = nullptr;
    mu2e::HitCount* hitcount = nullptr;
    mu2e::CrvSummaryReco* crvsummary = nullptr;
    mu2e::CrvSummaryMC* crvsummarymc = nullptr;
    mu2e::TrigInfo triginfo; // not a pointer because we give the address of array elements inside this
    Trigger trigger; // contains additional trigger information

    std::vector<mu2e::TrkInfo>* trk = nullptr;
    std::vector<mu2e::TrkInfoMC>* trkmc = nullptr;
    std::vector<mu2e::TrkDtDtInfo>* trkdtdt = nullptr;
    std::vector<mu2e::TrkCaloHitInfo>* trkcalohit = nullptr;
    std::vector<mu2e::CaloClusterInfoMC>* trkcalohitmc = nullptr;
    std::vector<mu2e::MVAResultInfo>* trkqual = nullptr;
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
    std::vector<std::shared_ptr<UserBranchBase>> user_branches;

    std::vector<mu2e::EventNtupleTimeClusterInfo>* timeclusters = nullptr;
    std::vector<mu2e::LineSeedInfo>* lineseeds = nullptr;

    // Every time cluster / line seed branch found by class, keyed by output branch name
    // (includes "timeclusters"/"lineseeds", aliased to the pointers above in Update()).
    std::map<std::string, std::vector<mu2e::EventNtupleTimeClusterInfo>*> timecluster_branches;
    std::map<std::string, std::vector<mu2e::LineSeedInfo>*> lineseed_branches;
    std::map<std::string, TimeClusters> named_time_clusters;
    std::map<std::string, LineSeeds> named_line_seeds;
    std::vector<std::vector<mu2e::EventNtupleComboHitInfo>>* timeclustershits = nullptr;

    // The companion "<collection>hits" branches, keyed by the *collection* name rather than the
    // branch name, so they line up with the maps above ("timeclusters" -> the "timeclustershits"
    // branch, aliased to the dedicated pointer above in BuildTimeClusters()).
    HitBranches timecluster_hit_branches;
    HitBranches lineseed_hit_branches;

    std::vector<mu2e::CaloClusterInfo>* caloclusters = nullptr;
    std::vector<mu2e::CaloHitInfo>* calohits = nullptr;
    std::vector<mu2e::CaloRecoDigiInfo>* calorecodigis = nullptr;
    std::vector<mu2e::CaloDigiInfo>* calodigis = nullptr;
    std::vector<mu2e::CaloClusterInfoMC>* caloclustersmc = nullptr;
    std::vector<mu2e::CaloHitInfoMC>* calohitsmc = nullptr;

    std::vector<mu2e::CrvHitInfoReco>* crvcoincs = nullptr;
    std::vector<mu2e::CrvHitInfoMC>* crvcoincsmc = nullptr;
    std::vector<mu2e::CrvWaveformInfo>* crvdigis = nullptr;
    std::vector<mu2e::CrvPulseInfoReco>* crvpulses = nullptr;
    std::vector<mu2e::CrvHitInfoMC>* crvpulsesmc = nullptr;
    std::vector<mu2e::CrvPlaneInfoMC>* crvcoincsmcplane = nullptr;

    std::vector<std::vector<mu2e::SimInfo>>* trkmcsim = nullptr;
    std::vector<mu2e::MCStepInfo>* mcsteps_virtualdetector = nullptr; // TODO: EventNtuple could have other mcsteps_* branches but for the time being just hardcode for the virtualdetector ones
    std::vector<mu2e::SimInfo>* calomcsim = nullptr;
  };
} // namespace rooutil
#endif
