//
// An example of how to plot the initial z-position of MCParticle muons
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

void PlotMuonPosZ(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hMuonPosZ = new TH1F("hMuonPosZ", "", 6000,4000,10000);

  // Set up RooUtil
  RooUtil util(filename);
  //  util.Debug(true);
  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto e_minus_tracks = event.GetTracks(is_e_minus);

    // Loop through the e_minus tracks
    for (auto& track : e_minus_tracks) {

      // Get the track segments at the tracker entrance and has an MC step
      auto mu_particles = track.GetMCParticles(is_muon);

      // Loop through the tracker entrance track segments
      for (auto& particle : mu_particles) {

        // Fill the histogram
        hMuonPosZ->Fill(particle.mcsim->pos.z());
      }
    }
  }

  // Draw the histogram
  hMuonPosZ->Draw("HIST E");
}
