//
// An example of how to plot the electron vs. muon track dt/dt fit results
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

using namespace rooutil;
void PlotTrkDtDt(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hE = new TH1F("h_e_dtdt", "Track dt_{hit} / dt_{trk};dt_{hit}/dt_{trk};Entries", 50,-2.,4.);
  TH1F* hM = new TH1F("h_m_dtdt", "Track dt_{hit} / dt_{trk};dt_{hit}/dt_{trk};Entries", 50,-2.,4.);

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e- and mu- tracks from the event
    auto e_minus_tracks  = event.GetTracks(is_e_minus );
    auto mu_minus_tracks = event.GetTracks(is_mu_minus);

    // Loop through the e- tracks
    for (auto& track : e_minus_tracks) {
      // Fill the histogram
      if (track.trkdtdt) hE->Fill(track.trkdtdt->slope);
    }

    // Loop through the mu- tracks
    for (auto& track : mu_minus_tracks) {
      // Fill the histogram
      if (track.trkdtdt) hM->Fill(track.trkdtdt->slope);
    }
  }

  // Draw the histogram
  hE->Draw("HIST");
  hM->SetLineColor(kRed);
  hM->SetMarkerStyle(20);
  hM->Draw("E1 SAMES");
  hE->GetYaxis()->SetRangeUser(0.1, 1.2*max(hE->GetMaximum(), hM->GetMaximum()));

  auto leg = new TLegend();
  leg->AddEntry(hE, "Electron fit", "L");
  leg->AddEntry(hM, "Muon fit", "PE");
  leg->Draw();
}
