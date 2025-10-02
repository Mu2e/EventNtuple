//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH2F.h"

void PlotTrackHitCalibs(std::string filename) {

  // Create the histogram you want to fill
  const int n_planes = 6;
  TH2F* hTrackHitCalib_dDdPlane = new TH2F("hTrackHitCalib_dDdPlane", "Track Hit Calibs", 100,0,1, n_planes,0,n_planes);

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
        if (trkhit.calib) {
          for (int i_plane = 0; i_plane < n_planes; ++i_plane) {
            hTrackHitCalib_dDdPlane->Fill(trkhit.calib->dDdPlane[i_plane], i_plane);
          }
        }
      }
    }
  }

  // Draw the histogram
  hTrackHitCalib_dDdPlane->Draw("COLZ");
}
