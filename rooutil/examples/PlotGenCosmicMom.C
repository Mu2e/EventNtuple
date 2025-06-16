//
// An example of how to plot the initial z-position of MCParticle muons
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

void PlotGenCosmicMom(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hGenCosmicMom = new TH1F("hGenCosmicMom", "", 100,0,5000);

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
      auto gen_cosmics = track.GetMCParticles(is_cosmic);
      // Loop through the tracker entrance track segments
      for (auto& particle : gen_cosmics) {
        // Fill the histogram
        hGenCosmicMom->Fill(particle.mcsim->mom.R());
      }
    }
  }

  // Draw the histogram
  hGenCosmicMom->SetLineColor(kBlue);
  hGenCosmicMom->Draw("HIST E");
}
