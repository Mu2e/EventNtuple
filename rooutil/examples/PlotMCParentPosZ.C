//
// An example of how to plot the initial z-position of MCParticle muons
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

void PlotMCParentPosZ(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hParentPosZ = new TH1F("hParentPosZ", "", 700,3000,10000);

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
    std::cout << "AE: n_e_minus_tracks = " << e_minus_tracks.size() << std::endl;
    for (auto& track : e_minus_tracks) {

      // Get the track segments at the tracker entrance and has an MC step
      auto parent_particles = track.GetMCParticles(is_track_parent);
      // Loop through the tracker entrance track segments
      std::cout << "AE: n_parents = " << parent_particles.size() << std::endl;
      for (auto& particle : parent_particles) {
        // Fill the histogram
        hParentPosZ->Fill(particle.mcsim->pos.z());
      }
    }
  }

  // Draw the histogram
  hParentPosZ->Draw("HIST E");
}
