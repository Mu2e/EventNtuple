//
// An example of how to plot reco vs true
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH2F.h"

using namespace rooutil;
void PlotTrackNHits_RecoVsTrue(std::string filename) {

  // Create the histogram you want to fill
  TH2F* hNHits_RecoVsTrue = new TH2F("hNHits_RecoVsTrue", "Number of Hits on Track (Reco vs True)", 100,0,100, 100,0,100);
  hNHits_RecoVsTrue->SetXTitle("True N Hits");
  hNHits_RecoVsTrue->SetYTitle("Reco N Hits");

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
      hNHits_RecoVsTrue->Fill(track.trkmc->nhits, track.trk->nhits);
    }
  }

  // Draw the histogram
  hNHits_RecoVsTrue->Draw("COLZ");
}
