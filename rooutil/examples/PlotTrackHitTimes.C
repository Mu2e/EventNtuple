//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

void PlotTrackHitTimes(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hTrackHitTimes = new TH1F("hTrackHitTimes", "Track Hit Times", 170,0,1700);

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto e_minus_tracks = event.GetTracks(is_e_minus);

    // Loop through the e_minus tracks
    for (auto& track : e_minus_tracks) {

      // Get the track segments at the tracker entrance
      auto trkhits = track.GetHits();

      // Loop through the tracker entrance track segments
      for (auto& trkhit : trkhits) {

        // Fill the histogram
        hTrackHitTimes->Fill(trkhit.reco->ptoca);
      }
    }
  }

  // Draw the histogram
  hTrackHitTimes->Draw("HIST E");
}
