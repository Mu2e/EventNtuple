//
// An example of how to plot CRV information
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

void PlotCRVPEs(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hPEs = new TH1F("hPEs", "Total PEs per Crv Coincidence", 100,0,200);
  TH1F* hPEs_3of4 = new TH1F("hPEs_3of4", "Total PEs per Crv Coincidence (for 3 of 4 layers hits)", 100,0,200);

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    auto crv_coincs = event.GetCrvCoincs();
    for (auto& crv_coinc : crv_coincs) {
      // Fill the histogram
      hPEs->Fill(crv_coinc.reco->PEs);
    }

    auto crv_selected_coincs = event.GetCrvCoincs(three_of_four_coinc);
    for (auto& crv_coinc : crv_selected_coincs) {
      // Fill the histogram
      hPEs_3of4->Fill(crv_coinc.reco->PEs);
    }

  }

  // Draw the histogram
  hPEs->Draw("HIST E");
  hPEs_3of4->SetLineColor(kRed);
  hPEs_3of4->Draw("HIST E SAMES");
}
