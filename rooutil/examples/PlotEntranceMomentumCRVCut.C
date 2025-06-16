//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

#include "TH1F.h"
#include "TCanvas.h"

void PlotEntranceMomentumCRVCut(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hRecoMom = new TH1F("hRecoMom", "Reconstructed Momentum at Tracker Entrance", 50,95,110);
  TH1F* h_dt = new TH1F("h_dt", "Reconstructed Momentum at Tracker Entrance", 100,-500,500);
  TH1F* h_best_dt = new TH1F("h_best_dt", "Reconstructed Momentum at Tracker Entrance", 100,-500,500);

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto e_minus_tracks = event.GetTracks(is_e_minus);

    auto crv_coincs = event.GetCrvCoincs();
    // Loop through the e_minus tracks
    for (auto& track : e_minus_tracks) {

      // Get the track segments at the tracker entrance
      auto trk_ent_segments = track.GetSegments([](TrackSegment& segment) { return tracker_entrance(segment) && has_reco_step(segment); });

      // Loop through the tracker entrance track segments
      for (auto& segment : trk_ent_segments) {
        // Find the crvcoinc closest in time to this segment
        int i_best_crv = -1;
        double min_dt = 9999;
        for (int i_crv_coinc = 0; i_crv_coinc < event.nCrvCoincs(); ++i_crv_coinc) {
          auto crv_coinc = crv_coincs.at(i_crv_coinc);
          double dt = segment.trkseg->time -  crv_coinc.reco->time;
          h_dt->Fill(dt);
          if (dt > 200 & dt < min_dt) {
            min_dt = dt;
            i_best_crv = i_crv_coinc;
          }
        }
        if (i_best_crv >= 0) {
          auto crv_coinc = crv_coincs.at(i_best_crv);
          double dt = segment.trkseg->time -  crv_coinc.reco->time;
          h_best_dt->Fill(dt);
          if (!track_crv_coincidence(segment, crv_coinc)) { // don't want track and crv hit to be coincident
            // Fill the histogram
            hRecoMom->Fill(segment.trkseg->mom.R());
          }
        }
        else {
          hRecoMom->Fill(segment.trkseg->mom.R());
        }
      }
    }
  }

  // Draw the histogram
  TCanvas* c = new TCanvas("c", "c");
  c->Divide(2);
  c->cd(1);
  h_dt->Draw("HIST E");
  h_best_dt->SetFillColor(kRed);
  h_best_dt->Draw("HIST E SAME");
  c->cd(2);
  hRecoMom->Draw("HIST E");
}
