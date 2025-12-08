//
// An example of how to plot reco vs true
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

using namespace rooutil;
void PlotCaloClusterEnergy(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hCaloClusterEnergy = new TH1F("hCaloClusterEnergy", "Energy (all clusters)", 110,0,110);
  hCaloClusterEnergy->SetXTitle("Energy [MeV]");

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto calo_clusters = event.GetCaloClusters();

    // Loop through the e_minus tracks
    for (auto& calo_cluster : calo_clusters) {
      // Fill the histogram
      hCaloClusterEnergy->Fill(calo_cluster.calocluster->energyDep_);
    }
  }

  // Draw the histogram
  hCaloClusterEnergy->Draw("HIST E");
}
