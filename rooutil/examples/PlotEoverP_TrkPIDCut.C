//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"
#include "TCanvas.h"

using namespace rooutil;
void PlotEoverP_TrkPIDCut(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hEoverP_All = new TH1F("hEoverP_All", "E / p", 200,0, 1);
  TH1F* hEoverP_TrkPIDCut = new TH1F("hEoverP_TrkPIDCut", "E / p", 200,0, 1);

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

      double EoverP = track.trkcalohit->edep  / track.trkcalohit->mom.R();
      hEoverP_All->Fill(EoverP);

      if (passes_trkpid(track, 0.8)) {
        hEoverP_TrkPIDCut->Fill(EoverP);
      }
    }
  }

  // Draw the histogram
  TCanvas* c1 = new TCanvas();
  hEoverP_All->SetLineColor(kBlack);
  hEoverP_All->Draw("HIST E");

  hEoverP_TrkPIDCut->SetLineColor(kRed);
  hEoverP_TrkPIDCut->Draw("HIST E SAME");
}
