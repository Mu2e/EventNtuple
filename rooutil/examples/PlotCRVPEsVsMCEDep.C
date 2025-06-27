//
// An example of how to plot CRV information
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH2F.h"

void PlotCRVPEsVsMCEDep(std::string filename) {

  // Create the histogram you want to fill
  TH2F* hPEs_MCEDep = new TH2F("hPEs_MCEDep", "Total PEs per Crv Coincidence", 100,0,300, 100,0,300);

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    auto crv_coincs = event.GetCrvCoincs();
    for (auto& crv_coinc : crv_coincs) {
      // Fill the histogram
      hPEs_MCEDep->Fill(crv_coinc.reco->PEs, crv_coinc.mc->depositedEnergy);
    }
  }

  // Draw the histogram
  hPEs_MCEDep->Draw("COLZ");
}
