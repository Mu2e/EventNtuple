//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

void PlotTrackMaterials(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hTrackMaterials_RadLen_Used = new TH1F("hTrackHitMaterials_RadLen_Used", "Used Track Materials", 200,0,2e-3);
  TH1F* hTrackMaterials_RadLen_Unused = new TH1F("hTrackHitMaterials_RadLen_Unused", "Unused Track Materials", 200,0,2e-3);

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
      auto trkmats = track.trkmats;

      // Loop through the tracker entrance track segments
      for (auto& trkmat : *trkmats) {

        // Fill the histogram
        if (trkmat.active == true) {
          hTrackMaterials_RadLen_Used->Fill(trkmat.radlen);
        }
        else {
          hTrackMaterials_RadLen_Unused->Fill(trkmat.radlen);
        }
      }
    }
  }

  // Draw the histogram
  hTrackMaterials_RadLen_Used->SetLineColor(kRed);
  hTrackMaterials_RadLen_Unused->SetLineColor(kBlack);
  hTrackMaterials_RadLen_Used->Draw("HIST E");
  hTrackMaterials_RadLen_Unused->Draw("HIST E SAME");
}
