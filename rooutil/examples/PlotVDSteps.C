//
// An example of how to plot reco vs true
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace rooutil;
void PlotVDSteps(std::string filename) {

  // Create the histogram you want to fill
  TH2F* hVD13_XY = new TH2F("hVD13_XY", "XY position at VD13 (tracker entrance)", 100,-500,500, 100,-500,500);
  hVD13_XY->SetXTitle("X (det. coords) [mm]");
  hVD13_XY->SetYTitle("Y (det. coords) [mm]");

  TH1F* hVD13_Mom = new TH1F("hVD13_Mom", "Momentum at VD13 (tracker entrance)", 120,0,120);
  hVD13_Mom->SetXTitle("Momentum [MeV/c]");

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    if (event.mcsteps_virtualdetector != nullptr) {
      for (const auto& vdstep : *(event.mcsteps_virtualdetector)) {
        if (vdstep.vid == mu2e::VirtualDetectorId::TT_FrontHollow) {
          // Fill the histogram
          hVD13_XY->Fill(vdstep.pos.x(), vdstep.pos.y());

          hVD13_Mom->Fill(vdstep.mom.R());
        }
      }
    }
  }

  // Draw the histogram
  TCanvas* c = new TCanvas();
  c->Divide(2);
  c->cd(1);
  hVD13_XY->Draw("COLZ");
  c->cd(2);
  hVD13_Mom->Draw("HIST E");
}
