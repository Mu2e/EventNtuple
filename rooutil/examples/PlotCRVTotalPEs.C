//
// An example of how to plot CRV information
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

void PlotCRVTotalPEs(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hTotalPEs = new TH1F("hTotalPEs", "Total PEs from crvsummary", 100,0,200);

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    hTotalPEs->Fill(event.crvsummary->totalPEs);
  }

  // Draw the histogram
  hTotalPEs->Draw("HIST E");
}
