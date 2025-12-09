//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

using namespace rooutil;
void PlotEntranceMomentum_UpstreamDownstream(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hRecoMomDown = new TH1F("hRecoMomDown", "Reconstructed Momentum at Tracker Entrance (downstream)", 50,95,110);
  TH1F* hRecoMomUp = new TH1F("hRecoMomUp", "Reconstructed Momentum at Tracker Entrance (upstream)", 50,95,110);

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the downstream e_minus tracks from the event
    auto dwn_e_minus_tracks = event.GetTracks([](Track& track){ return is_e_minus(track) && is_downstream(track); });
    auto up_e_minus_tracks = event.GetTracks([](Track& track){ return is_e_minus(track) && is_upstream(track); });

    // Loop through the dwn_e_minus tracks
    for (auto& track : dwn_e_minus_tracks) {

      // Get the track segments at the tracker entrance
      auto trk_ent_segments = track.GetSegments([](TrackSegment& segment){ return tracker_entrance(segment) && has_reco_step(segment); });

      // Loop through the tracker entrance track segments
      for (auto& segment : trk_ent_segments) {

        // Fill the histogram
        hRecoMomDown->Fill(segment.trkseg->mom.R());
      }
    }

    // Loop through the up_e_minus tracks
    for (auto& track : up_e_minus_tracks) {

      // Get the track segments at the tracker entrance
      auto trk_ent_segments = track.GetSegments([](TrackSegment& segment){ return tracker_entrance(segment) && has_reco_step(segment); });

      // Loop through the tracker entrance track segments
      for (auto& segment : trk_ent_segments) {

        // Fill the histogram
        hRecoMomUp->Fill(segment.trkseg->mom.R());
      }
    }
  }

  // Draw the histogram
  hRecoMomDown->Draw("HIST E");
  hRecoMomUp->SetLineColor(kRed);
  hRecoMomUp->Draw("HIST E SAME");
}
