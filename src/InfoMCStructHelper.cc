//
// Namespace for collecting tools used in MC truth evaluation
// Original author: Dave Brown (LBNL) 8/10/2016
//
#include "EventNtuple/inc/InfoMCStructHelper.hh"
#include "Offline/MCDataProducts/inc/StepPointMC.hh"
#include "Offline/MCDataProducts/inc/SimParticle.hh"
#include "Offline/MCDataProducts/inc/MCRelationship.hh"

#include "Offline/TrackerGeom/inc/Tracker.hh"
#include "Offline/Mu2eUtilities/inc/TwoLinePCA.hh"
#include "Offline/DataProducts/inc/EventWindowMarker.hh"
#include "Offline/DataProducts/inc/VirtualDetectorId.hh"

#include "Offline/GlobalConstantsService/inc/GlobalConstantsHandle.hh"
#include "Offline/GlobalConstantsService/inc/ParticleDataList.hh"
#include "Offline/GlobalConstantsService/inc/PhysicsParams.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"
#include "Offline/GeometryService/inc/DetectorSystem.hh"
#include "Offline/TrackerGeom/inc/Tracker.hh"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Event.h"

#include <map>
#include <limits>

namespace mu2e {

  InfoMCStructHelper::InfoMCStructHelper(const Config& conf) :
    _spctag(conf.spctag()), _mingood(conf.mingood()),
    _maxdt(conf.maxvddt()),
    _ewMarkerTag(conf.ewMarkerTag()) {
      // build the VDId -> SId map by hand
      _vdmap[VirtualDetectorId(VirtualDetectorId::TT_FrontHollow)] = SurfaceId("TT_Front");
      _vdmap[VirtualDetectorId(VirtualDetectorId::TT_Mid)] = SurfaceId("TT_Mid");
      _vdmap[VirtualDetectorId(VirtualDetectorId::TT_MidInner)] = SurfaceId("TT_Mid");
      _vdmap[VirtualDetectorId(VirtualDetectorId::TT_Back)] = SurfaceId("TT_Back");
      _vdmap[VirtualDetectorId(VirtualDetectorId::TT_OutSurf)] = SurfaceId("TT_Outer");
      _vdmap[VirtualDetectorId(VirtualDetectorId::TT_InSurf)] = SurfaceId("TT_Inner");
    }

  void InfoMCStructHelper::updateEvent(const art::Event& event) {
    event.getByLabel(_spctag,_spcH);
    _mbtime = GlobalConstantsHandle<PhysicsParams>()->getNominalDRPeriod();
    art::Handle<EventWindowMarker> ewMarkerHandle;
    event.getByLabel(_ewMarkerTag, ewMarkerHandle);
    const EventWindowMarker& ewMarker(*ewMarkerHandle);
    _onSpill = (ewMarker.spillType() == EventWindowMarker::SpillType::onspill);
  }

  void InfoMCStructHelper::fillTrkInfoMC(const KalSeed& kseed, const KalSeedMC& kseedmc, art::Handle<SurfaceStepCollection> surfaceStepsHandle, std::vector<TrkInfoMC>& all_trkinfomcs) {
    // use the primary match of the track
    // primary associated SimParticle
    TrkInfoMC trkinfomc;
    GeomHandle<DetectorSystem> det;
    if(kseedmc.simParticles().size() > 0){
      auto const& simp = kseedmc.simParticles().front();
      trkinfomc.valid = true;
      trkinfomc.nhits = simp._nhits; // number of hits from the primary particle
      trkinfomc.nactive = simp._nactive; // number of active hits from the primary particle

      static GlobalConstantsHandle<ParticleDataList> pdt;
      auto charge = pdt->particle(simp._pdg).charge();

      XYZTVectorF mom = XYZTVectorF(simp._mom);
      CLHEP::Hep3Vector posInMu2e(simp._pos.x(), simp._pos.y(), simp._pos.z());
      XYZVectorF pos = XYZVectorF(det->toDetector(posInMu2e));
      ROOT::Math::XYZTVector pos0(pos.x(), pos.y(), pos.z(), simp._pos.t());
      ROOT::Math::PxPyPzMVector mom0(mom.x(), mom.y(), mom.z(),  pdt->particle(simp._pdg).mass());

      GeomHandle<BFieldManager> bfmgr;
      mu2e::GeomHandle<mu2e::Tracker> tracker;
      auto tracker_origin = det->toMu2e(tracker->origin());
      XYZVectorF pos3Vec = XYZVectorF(tracker_origin.x(),tracker_origin.y(),tracker_origin.z());
      ROOT::Math::XYZVector bnom(bfmgr->getBField(pos3Vec).x(),bfmgr->getBField(pos3Vec).y(),bfmgr->getBField(pos3Vec).z());
      //XYZVectorF pos_in_Mu2e = XYZVectorF(simp._pos);
      //ROOT::Math::XYZVector bnom(bfmgr->getBField(pos_in_Mu2e).x(),bfmgr->getBField(pos_in_Mu2e).y(),bfmgr->getBField(pos_in_Mu2e).z());
      KinKal::LoopHelix lh(pos0, mom0, charge, bnom);
      trkinfomc.maxr =sqrt(lh.cx()*lh.cx()+lh.cy()*lh.cy())+fabs(lh.rad());
      trkinfomc.rad = lh.rad();
      trkinfomc.lam = lh.lam();
      trkinfomc.cx = lh.cx();
      trkinfomc.cy = lh.cy();
      trkinfomc.phi0= lh.phi0();
      trkinfomc.t0 = lh.t0();
      // SurfaceStep counts
      const static SurfaceId ipasid(SurfaceIdDetail::IPA);
      const static SurfaceId stsid(SurfaceIdDetail::ST_Foils,-1);
      if(surfaceStepsHandle.isValid()){
        auto const& surfsteps = *surfaceStepsHandle;
        for (auto const& ss : surfsteps) {
          if(ss.simParticle()->id() == simp._spkey){
            if(ss.surfaceId() == ipasid){
              if(ss.momentum().Z() > 0)
                ++trkinfomc.nipadown;
              else
                ++trkinfomc.nipaup;
            }
            if(ss.surfaceId() == stsid){
              if(ss.momentum().Z() > 0)
                ++trkinfomc.nstdown;
              else
                ++trkinfomc.nstup;
            }
          }
        }
      }
    }

    fillTrkInfoMCDigis(kseed, kseedmc, trkinfomc);
    all_trkinfomcs.push_back(trkinfomc);
  }

  void InfoMCStructHelper::fillTrkInfoMCDigis(const KalSeed& kseed, const KalSeedMC& kseedmc, TrkInfoMC& trkinfomc) {
    trkinfomc.ndigi = 0; trkinfomc.ndigigood = 0, trkinfomc.nambig = 0;
    // find the first segment momentum as reference
    double simmom = 1.0;
    if(kseedmc.simParticles().size()>0)
      simmom = kseedmc.simParticles().front()._mom.R();
    for(size_t i_digi = 0; i_digi < kseedmc._tshmcs.size(); ++i_digi) {
      const auto& tshmc = kseedmc._tshmcs.at(i_digi);
      if (kseedmc.simParticle(tshmc._spindex)._rel == MCRelationship::same) {
        ++trkinfomc.ndigi;
        if(tshmc.particleMomentum().R()/simmom > _mingood) {
          ++trkinfomc.ndigigood;
        }
        // easiest way to get MC ambiguity is through info object
        TrkStrawHitInfoMC tshinfomc;
        fillHitInfoMC(kseedmc,tshinfomc,tshmc);
        // the MCDigi list can be longer than the # of TrkStrawHits in the seed:
        if(i_digi < kseed.hits().size()){
          const auto& ihit = kseed.hits().at(i_digi);
          if(ihit.flag().hasAllProperties(StrawHitFlag::active) && ihit.ambig()*tshinfomc.ambig > 0) {
            ++trkinfomc.nambig;
          }
        }
      }
    }
  }

  void InfoMCStructHelper::fillHitInfoMC(const KalSeedMC& kseedmc, TrkStrawHitInfoMC& tshinfomc, const TrkStrawHitMC& tshmc) {
    auto const& simPart = kseedmc.simParticle(tshmc._spindex);
    tshinfomc.pdg = simPart._pdg;
    tshinfomc.startCode = simPart._proc;
    tshinfomc.gen = simPart._gid.id();
    tshinfomc.rel = simPart._rel;
    tshinfomc.earlyend = tshmc._earlyend._end;
    tshinfomc.t0 = tshmc._time;
    tshinfomc.tdrift = tshmc._tdrift;
    tshinfomc.rdrift = tshmc._rdrift;
    tshinfomc.tprop = tshmc._tprop;
    tshinfomc.edep = tshmc._energySum;
    tshinfomc.mom = tshmc._mom.R();
    tshinfomc.cpos  = tshmc._cpos;
    tshinfomc.len = tshmc._wireLen;
    tshinfomc.twdot = tshmc._wireDot;
    tshinfomc.doca = tshmc._wireDOCA;
    tshinfomc.dist = fabs(tshmc._wireDOCA);
    tshinfomc.strawdoca = tshmc._strawDOCA;
    tshinfomc.phi = tshmc._wirePhi;
    tshinfomc.strawphi = tshmc._strawPhi;
    tshinfomc.lang = fabs(fmod(tshmc._wirePhi,M_PI));
    if (tshinfomc.lang > M_PI_2) tshinfomc.lang = M_PI - tshinfomc.lang;
    tshinfomc.lang = M_PI_2 - tshinfomc.lang; // currently in tshinfo lang is calculated perpendicular to drift direction
    tshinfomc.tau = tshmc._wireTau;
    tshinfomc.cdist = sqrt(tshinfomc.doca*tshinfomc.doca+tshinfomc.tau*tshinfomc.tau);
    tshinfomc.ambig = tshmc._wireDOCA > 0 ? 1 : -1;
  }

  void InfoMCStructHelper::fillAllSimInfos(const KalSeedMC& kseedmc, const PrimaryParticle& primary, std::vector<std::vector<SimInfo>>& all_siminfos, int n_generations, int n_match) {
    std::vector<SimInfo> siminfos;

    // interpret -1 as no llimit
    if (n_generations == -1) {
      n_generations = std::numeric_limits<int>::max();
    }
    if (n_match == -1 ) {
      n_match = std::numeric_limits<int>::max();
    }

    //create vector of art Ptr to particles:
    std::vector<art::Ptr<SimParticle> > allParts;

    //loop over all sime particles in KalSeedMC
    for(int imatch = 0 ; imatch < std::min(n_match,static_cast<int>(kseedmc.simParticles().size())); ++imatch) {
      auto trkprimaryptr = kseedmc.simParticle(imatch).simParticle(_spcH);
      auto trkprimary = trkprimaryptr->originParticle();
      auto current_sim_particle_ptr = trkprimaryptr;
      auto current_sim_particle = trkprimary;

      for (int i_generation = 0; i_generation < n_generations; ++i_generation) {
        bool isSame = false;
        for(unsigned int ipart = 0; ipart < allParts.size() ; ipart++){
          MCRelationship checkrel(current_sim_particle_ptr,allParts.at(ipart));
          if(checkrel==MCRelationship::same) {
              isSame = true;
            }
        }
        if(!isSame) { allParts.push_back(current_sim_particle_ptr); }

        SimInfo sim_info;
        fillSimInfo(current_sim_particle, sim_info);
        sim_info.trkrel = MCRelationship(current_sim_particle_ptr, trkprimaryptr);
        sim_info.rank = imatch;

        auto bestprimarysp = primary.primarySimParticles().front();
        MCRelationship bestrel;
        for(auto const& spp : primary.primarySimParticles()){
          MCRelationship mcrel(current_sim_particle_ptr, spp);
          if(mcrel > bestrel){
            bestrel = mcrel;
            bestprimarysp = spp;
          }
        }
        sim_info.prirel = bestrel;
        // only count hits for direct contributors
        if(i_generation == 0){
          sim_info.nhits = kseedmc.simParticle(imatch)._nhits;
          sim_info.nactive = kseedmc.simParticle(imatch)._nactive;
        }
        else { // don't set a rank for ancestor particles
          sim_info.rank = -1;
        }
        // record the index this object will have
        sim_info.index = siminfos.size();

        if(!isSame) {
          siminfos.push_back(sim_info);
        }

        if (current_sim_particle.parent().isNonnull()) {
          current_sim_particle_ptr = current_sim_particle.parent();
          current_sim_particle = current_sim_particle_ptr->originParticle();
        }
        else {
          break; // this particle doesn't have a parent
        }
      }

    }

    // Now add all the primary particles
    SimInfo sim_info;
    for(auto const& spp : primary.primarySimParticles()){
      fillSimInfo(spp, sim_info);

      // check whether we already put this primary in
      bool already_added = false;
      for (const auto& i_sim_info : siminfos) {
        if (i_sim_info.prirel == MCRelationship::same) {
          already_added = true;
          break;
        }
      }
      if (!already_added) {
        auto trkprimaryptr = kseedmc.simParticle().simParticle(_spcH);
        sim_info.trkrel = MCRelationship(spp, trkprimaryptr);
        sim_info.prirel = MCRelationship(spp, spp);
        sim_info.index = siminfos.size();
        siminfos.push_back(sim_info);
      }
    }

    all_siminfos.push_back(siminfos);
  }


  void InfoMCStructHelper::fillSimInfo(const art::Ptr<SimParticle>& sp, SimInfo& siminfo) {
    if(sp.isNonnull()){
      fillSimInfo(*sp, siminfo);
    }
    else {
      siminfo.valid = false;
    }
  }

  void InfoMCStructHelper::fillSimInfo(const SimParticle& sp, SimInfo& siminfo) {
    GeomHandle<DetectorSystem> det;
    siminfo.valid = true;
    siminfo.id = sp.id().asInt();
    if(sp.genParticle().isNonnull()) { siminfo.gen = sp.genParticle()->generatorId().id(); }
    siminfo.startCode = sp.creationCode();
    siminfo.stopCode = sp.stoppingCode();
    siminfo.pdg = sp.pdgId();
    siminfo.time = sp.startGlobalTime();
    siminfo.mom = XYZVectorF(sp.startMomentum());
    siminfo.pos = XYZVectorF(det->toDetector(sp.startPosition()));
    siminfo.endpos = XYZVectorF(det->toDetector(sp.endPosition()));
    siminfo.endmom = XYZVectorF(sp.endMomentum());
  }

  void InfoMCStructHelper::fillVDInfo(const KalSeed& kseed, const KalSeedMC& kseedmc, std::vector<std::vector<MCStepInfo>>& all_vdinfos) {
    std::vector<MCStepInfo> vdinfos;
    const auto& vdsteps = kseedmc._vdsteps;
    const auto& inters = kseed.intersections();
    double tmin = std::numeric_limits<float>::max();
    double tmax = std::numeric_limits<float>::lowest();
    size_t imin(0), imax(0);

    for (const auto& vdstep : vdsteps) {
      // record VD steps close in time and space with candidate samples (intersections)
      double corrected_time;
      if(_onSpill) {
        corrected_time = std::fmod(vdstep._time,_mbtime);
      } else {
        corrected_time = vdstep._time;
      }
      double dtmin = std::numeric_limits<float>::max();
      MCStepInfo vdinfo;
      for(size_t iinter=0; iinter < inters.size(); ++iinter){
        auto const& inter = inters[iinter];
        // make sure this is the same surface and that the particles are going in the same direction
        if(_vdmap[vdstep._vdid] == inter.surfaceId() && vdstep._mom.Dot(inter.intersection().pdir_) > 0.0){
          // there could still be multiple intersections if the particle reflected: check
          double dt = fabs(inter.time() - corrected_time);
          if(dt < _maxdt && dt < dtmin) {
            dtmin = dt;
            vdinfo.time = corrected_time; // use corrected time for early, late flagging.  Not sure if this affects anything
            vdinfo.mom = XYZVectorF(vdstep._mom);
            vdinfo.pos = XYZVectorF(vdstep._pos);
            vdinfo.vid = vdstep._vdid.id();
            vdinfo.iinter = iinter;
            vdinfo.sid = inter.surfaceId().id();
          }
        }
      }
      if(vdinfo.iinter >=0){
        vdinfos.push_back(vdinfo);
        if(vdinfo.time > tmax){
          tmax = vdinfo.time;
          imax = vdinfos.size()-1;
        }
        if(vdinfo.time < tmin){
          tmin = vdinfo.time;
          imin = vdinfos.size()-1;
        }
      }
    }
    if(vdinfos.size() > 0){
      vdinfos[imin].early = true;
      vdinfos[imax].late = true;
    }
    all_vdinfos.push_back(vdinfos);
  }

  void InfoMCStructHelper::fillHitInfoMCs(const KalSeed& kseed, const KalSeedMC& kseedmc, std::vector<std::vector<TrkStrawHitInfoMC>>& all_tshinfomcs) {
    unsigned nrecohit = kseed.hits().size();
    std::vector<TrkStrawHitInfoMC> tshinfomcs;
    for(size_t i_tshmc = 0; i_tshmc < kseedmc._tshmcs.size(); ++i_tshmc){
      auto const& tshmc = kseedmc._tshmcs[i_tshmc];
      TrkStrawHitInfoMC tshinfomc;
      fillHitInfoMC(kseedmc, tshinfomc, tshmc);
      tshinfomcs.push_back(tshinfomc);
      // mcdigis beyond the kalseed reco hit list are from the MC primary
      tshinfomcs.back().recohit = (i_tshmc < nrecohit);
    }
    all_tshinfomcs.push_back(tshinfomcs);
  }

  void InfoMCStructHelper::fillCaloClusterInfoMC(CaloClusterMC const& ccmc, std::vector<CaloClusterInfoMC>& ccimcs) {
    CaloClusterInfoMC ccimc;
    auto const& edeps = ccmc.energyDeposits();
    ccimc.nsim = edeps.size();
    ccimc.etot = ccmc.totalEnergyDep();
    if(ccmc.energyDeposits().size() > 0){
      auto const& primary = edeps.front();
      ccimc.eprimary = primary.energyDep();
      ccimc.tavg = primary.time(); // this is unnecessary FIXMI
      ccimc.tprimary = primary.time();
      ccimc.prel = primary.rel();
    }
    ccimcs.push_back(ccimc);
  }

  void InfoMCStructHelper::fillExtraMCStepInfos(KalSeedMC const& kseedmc, StepPointMCCollection const& mcsteps,
                                                std::vector<MCStepInfos>& mcsics, std::vector<MCStepSummaryInfo>& mcssis) {
    MCStepInfos mcsic;
    MCStepSummaryInfo mcssi;
    GeomHandle<DetectorSystem> det;
    MCStepInfo mcsi;
    // only count the extra steps associated with the primary MC truth match
    auto simp = kseedmc.simParticle().simParticle(_spcH);
    //    std::cout << "KalSeedMC simp kep " << simp.key()
    //      << " start mom " << simp->startMomentum().vect()
    //      << " pos " << simp->startPosition() << std::endl;
    for(auto const& mcstep : mcsteps) {
      //     std::cout << "MCStep simp key " << mcstep.simParticle().key()
      //       << " start mom " << mcstep.simParticle()->startMomentum().vect()
      //       << " pos " << mcstep.simParticle()->startPosition() << std::endl;
      if(mcstep.simParticle().key() == simp.key()){
        // combine steps if they are in the same material.  They must be close in time too, since the particle may
        // re-enter the same material
        auto spos = XYZVectorF(det->toDetector(mcstep.position()));
        if(static_cast<int>(mcstep.volumeId()) == mcsi.vid && fabs(mcstep.time()-mcsi.time)<0.1 ){
          mcsi.de += mcstep.totalEDep();
          mcsi.dp += mcstep.postMomentum().mag() - mcstep.momentum().mag();
        } else {
          if(mcsi.valid()){
            // save this step
            mcsic.push_back(mcsi);
            // add its info to the summary
            mcssi.addStep(mcsi);
          }
          // initialize the new step
          mcsi.reset();
          mcsi.vid = mcstep.volumeId();
          mcsi.time = mcstep.time();
          mcsi.de = mcstep.totalEDep();
          mcsi.dp = mcstep.postMomentum().mag()- mcstep.momentum().mag();
          mcsi.mom = mcstep.momentum();
          mcsi.pos = spos;
          mcsi.pdg = mcstep.simParticle()->pdgId();
          mcsi.startCode = mcstep.simParticle()->creationCode();
          mcsi.stopCode = mcstep.simParticle()->stoppingCode();
        }
      }
    }
    // finalize the last step
    if(mcsi.valid()){
      mcsic.push_back(mcsi);
      mcssi.addStep(mcsi);
    }
    mcsics.push_back(mcsic);
    mcssis.push_back(mcssi);
  }

  void InfoMCStructHelper::fillSurfaceStepInfos(KalSeedMC const& kseedmc, SurfaceStepCollection const& surfsteps,std::vector<SurfaceStepInfo>& ssic) {
    // only look at SurfaceSteps associated with the primary MC truth match particle
    auto simp = kseedmc.simParticle().simParticle(_spcH);
    for (auto const& ss : surfsteps) {
      if(ss.simParticle() == simp){
      //  std::cout << "Found matching surface step sid" << ss << " particle time " << simp->startGlobalTime() << std::endl;
        ssic.emplace_back(ss); // temporary
      }

    }
    //    std::sort(ssic.begin(),ssic.end(),[](const auto& a, const auto& b){return a.time < b.time;});
  }

  void InfoMCStructHelper::fillStepPointMCInfo(StepPointMCCollection const& mcsteps, MCStepInfos& mcstepinfos) {
    GeomHandle<DetectorSystem> det;
    MCStepInfo mcstepinfo;
    for(auto const& mcstep : mcsteps) {
      mcstepinfo.reset();
      mcstepinfo.vid = mcstep.volumeId();
      mcstepinfo.time = mcstep.time();
      mcstepinfo.de = mcstep.totalEDep();
      mcstepinfo.dp = mcstep.postMomentum().mag()- mcstep.momentum().mag();
      mcstepinfo.mom = mcstep.momentum();
      mcstepinfo.pos = XYZVectorF(det->toDetector(mcstep.position()));
      const auto& simParticle = mcstep.simParticle();
      mcstepinfo.pdg = simParticle->pdgId();
      mcstepinfo.startCode = simParticle->creationCode();
      mcstepinfo.stopCode = simParticle->stoppingCode();
      mcstepinfos.emplace_back(mcstepinfo);
    }
  }
}
