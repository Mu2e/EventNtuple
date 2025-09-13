//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

void PlotTrkCaloHitEnergy(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hTrkCaloHitEnergy = new TH1F("hTrkCaloHitEnergy", "TrkCaloHit Energy", 50,95,110);

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto e_minus_tracks = event.GetTracks(is_e_minus);

    // Loop through the e_minus tracks
    for (auto& track : e_minus_tracks) {

      // Fill the histogram
      if (track.trkcalohit->active) {
        hTrkCaloHitEnergy->Fill(track.trkcalohit->edep);
      }
    }
  }

  // Draw the histogram
  hTrkCaloHitEnergy->Draw("HIST E");
}
