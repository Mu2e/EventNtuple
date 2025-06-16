//
// This macro prints the a couple of leaves from each branch
// to test direct access to the branches via RooUtil's Event class
//
#include "EventNtuple/rooutil/inc/RooUtil.hh"

#include <iostream>

void PrintEvents(std::string filename) {

  RooUtil util(filename);
  //  util.Debug(true);
  std::cout << filename << " has " << util.GetNEvents() << " events" << std::endl;

  // Only print one event
  for (int i_event = 0; i_event < 1; ++i_event) {
    const auto& event = util.GetEvent(i_event);

    // evtinfo branch
    if (event.evtinfo != nullptr) {
      std::cout << "evtinfo: "<< event.evtinfo->run << "," << event.evtinfo->subrun << "," << event.evtinfo->event << "," << event.evtinfo->nprotons << "," << event.evtinfo->pbtime << "," << event.evtinfo->pbterr << std::endl;
    }

    // evtinfomc branch
    if (event.evtinfomc != nullptr) {
      std::cout << "evtinfomc: " << event.evtinfomc->nprotons << "," << event.evtinfomc->pbtime << std::endl;
    }

    // trk branch
    if (event.trk != nullptr) {
      for (const auto& trk : *(event.trk)) {
        std::cout << "trk: " << trk.status << "," << trk.nhits << "," << trk.nactive << "," << trk.avgedep << std::endl;
      }
    }

    // trkmc branch
    if (event.trkmc != nullptr) {
      for (const auto& trkmc : *(event.trkmc)) {
        std::cout << "trkmc: " << trkmc.valid << "," << trkmc.nhits << "," << trkmc.nactive << "," << trkmc.t0 << std::endl;
      }
    }

    // trksegs branch
    if (event.trksegs != nullptr) {
      for (const auto& trksegs : *(event.trksegs)) {
        std::cout << "New Track!" << std::endl;
        for (const auto& trkseg : trksegs) {
          std::cout << "trksegs: time = " << trkseg.time << " ns, p_mag = " << trkseg.mom.R() << " MeV/c, sid = " << trkseg.sid << ", p_z = " << trkseg.mom.z() << " MeV/c, z = " << trkseg.pos.z() << " mm, momerr = " << trkseg.momerr << " MeV/c" << std::endl;
        }
      }
    }

    // trksegsmc branch
    if (event.trksegsmc != nullptr) {
      for (const auto& trksegsmc : *(event.trksegsmc)) {
        std::cout << "New Track!" << std::endl;
        for (const auto& trksegmc : trksegsmc) {
          std::cout << "trksegsmc: t = " << trksegmc.time << " ns, p_mag = " << trksegmc.mom.R() << " MeV/c, sid = " << trksegmc.sid << ", p_z = " << trksegmc.mom.z() << " MeV/c, z = " << trksegmc.pos.z() << " mm" <<std::endl;
        }
      }
    }

    // trkcalohit branch
    if (event.trkcalohit != nullptr) {
      for (const auto& trkcalohit : *(event.trkcalohit)) {
        std::cout << "trkcalohit: " << trkcalohit.active << "," << trkcalohit.mom.R() << "," << trkcalohit.edep << "," << trkcalohit.edeperr << std::endl;
      }
    }

    // trkcalohitmc branch
    if (event.trkcalohitmc != nullptr) {
      for (const auto& trkcalohitmc : *(event.trkcalohitmc)) {
        std::cout << "trkcalohitmc: " << trkcalohitmc.nsim << "," << trkcalohitmc.etot << "," << trkcalohitmc.prel.relationship() << std::endl;
      }
    }


    // crvcoincs branch
    if (event.crvcoincs != nullptr) {
      for (const auto& crvcoinc : *(event.crvcoincs)) {
        std::cout << "crvcoinc: " << crvcoinc.sectorType << "," << crvcoinc.PEs << "," << crvcoinc.angle << std::endl;
        for (const auto& layer_PE : crvcoinc.PEsPerLayer) {
          std::cout << "crvcoinc layer PE: " << layer_PE << std::endl;
        }
      }
    }

    // crvcoincsmc branch
    if (event.crvcoincsmc != nullptr) {
      for (const auto& crvcoincmc : *(event.crvcoincsmc)) {
        std::cout << "crvcoincmc: " << crvcoincmc.valid << "," << crvcoincmc.pdgId << "," << crvcoincmc.depositedEnergy << std::endl;
      }
    }

    if (event.trkmcsim != nullptr) {
      for (const auto& trkmcsim : *(event.trkmcsim)) {
        std::cout << "New Track!" << std::endl;
        for (const auto& sim : trkmcsim) {
          std::cout << "trkmcsim: valid = " << sim.valid << ", nhits = " << sim.nhits << ", pdg = " << sim.pdg << ", p = " << sim.mom.R() << ", prirel = " << (int)sim.prirel.relationship() << ", trkrel = " << (int)sim.trkrel.relationship() << std::endl;
        }
      }
    }

    // trkqual branch
    if (event.trkqual != nullptr) {
      for (const auto& trkqual : *(event.trkqual)) {
        std::cout << "trkqual: " << trkqual.valid << ", " << trkqual.result << std::endl;
      }
    }

    // trksegpars_lh
    if (event.trksegpars_lh != nullptr) { // might not have this branch
      for (const auto& track : *(event.trksegpars_lh)) {
        std::cout << "New Track!" << std::endl;
        for (const auto& trksegpars : track) {
          std::cout << "trksegpars_lh: " << trksegpars.maxr << ", " << trksegpars.t0 << ", " << trksegpars.t0err << std::endl;
        }
      }
    }

    // trksegpars_ch
    if (event.trksegpars_ch != nullptr) { // might not have this branch
      for (const auto& track : *(event.trksegpars_ch)) {
        std::cout << "New Track!" << std::endl;
        for (const auto& trksegpars : track) {
          std::cout << "trksegpars_ch: " << trksegpars.d0 << ", " << trksegpars.t0 << ", " << trksegpars.t0err << ", " << trksegpars.maxr << std::endl;
        }
      }
    }
    // trksegpars_kl
    if (event.trksegpars_kl != nullptr) { // might not have this branch
      for (const auto& track : *(event.trksegpars_kl)) {
        std::cout << "New Track!" << std::endl;
        for (const auto& trksegpars : track) {
          std::cout << "trksegpars_kl: " << trksegpars.d0 << ", " << trksegpars.t0 << ", " << trksegpars.t0err << std::endl;
        }
      }
    }

    // trkhits
    if (event.trkhits != nullptr) { // might not have this branch
      for (const auto& trkhits : *(event.trkhits)) {
        std::cout << "New Track!" << std::endl;
        for (const auto& hit : trkhits) {
          std::cout << "trkhits: " << hit.plane << ", " << hit.panel << ", " << hit.layer << ", " << hit.straw << ", " << hit.dactive << std::endl;
        }
      }
    }

    // trkhitsmc
    if (event.trkhitsmc != nullptr) { // might not have this branch
      for (const auto& trkhitsmc : *(event.trkhitsmc)) {
        std::cout << "New Track!" << std::endl;
        for (const auto& hitmc : trkhitsmc) {
          std::cout << "trkhitsmc: " << hitmc.pdg << ", " << hitmc.startCode << ", " << hitmc.t0 << ", " << hitmc.cpos.z() << std::endl;
        }
      }
    }

    // trkmats
    if (event.trkmats != nullptr) { // might not have this branch
      for (const auto& trkmats : *(event.trkmats)) {
        std::cout << "New Track!" << std::endl;
        for (const auto& mat : trkmats) {
          std::cout << "trkmats: " << mat.active << ", " << mat.plane << ", " << mat.panel << ", " << mat.layer << ", " << mat.straw << ", " << mat.radlen << std::endl;
        }
      }
    }

    // hitcount
    if (event.hitcount != nullptr) {
      auto hitcount = *(event.hitcount);
      std::cout << "hitcount: " << hitcount.nsd << ", " << hitcount.nesel << ", " << hitcount.nrsel << ", " << hitcount.ntsel << ", " << hitcount.nbkg << std::endl;
    }

    // crvsummary
    if (event.crvsummary != nullptr) {
      auto crvsummary = *(event.crvsummary);
      std::cout << "crvsummary: " << crvsummary.totalPEs << ", " << crvsummary.nHitCounters << std::endl;
    }

    // crvsummarymc
    if (event.crvsummarymc != nullptr) {
      auto crvsummarymc = *(event.crvsummarymc);
      std::cout << "crvsummarymc: " << crvsummarymc.totalEnergyDeposited << ", " << crvsummarymc.minPathLayer << ", " << crvsummarymc.pdgId << std::endl;
    }
    // crvdigis
    if (event.crvdigis != nullptr) {
      for (const auto& crvdigi : *(event.crvdigis)) {
        std::cout << "crvdigi: " << crvdigi.adc << "," << crvdigi.time << "," << crvdigi.SiPMId << std::endl;
      }
    }

    // crvpulses branch
    if (event.crvpulses != nullptr) {
      for (const auto& crvpulse : *(event.crvpulses)) {
        std::cout << "crvpulse: " << crvpulse.pos.x() << "," << crvpulse.barId << "," << crvpulse.time << std::endl;
      }
    }

    // crvpulsesmc branch
    if (event.crvpulsesmc != nullptr) {
      for (const auto& crvpulsemc : *(event.crvpulsesmc)) {
        std::cout << "crvpulsemc: " << crvpulsemc.valid << "," << crvpulsemc.pdgId << "," << crvpulsemc.depositedEnergy << std::endl;
      }
    }

    // crvcoincsmcplane branch
    if (event.crvcoincsmcplane != nullptr) {
      for (const auto& crvcoincmcplane : *(event.crvcoincsmcplane)) {
        std::cout << "crvcoincmcplane: " << crvcoincmcplane.pdgId << "," << crvcoincmcplane.primary.z() << ", " << crvcoincmcplane.kineticEnergy << ", " << crvcoincmcplane.dataSource << std::endl;
      }
    }
  }
}
