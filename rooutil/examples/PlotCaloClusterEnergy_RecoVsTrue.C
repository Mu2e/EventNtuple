//
// An example of how to plot reco vs true
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH2F.h"

using namespace rooutil;
void PlotCaloClusterEnergy_RecoVsTrue(std::string filename) {

  // Create the histogram you want to fill
  TH2F* hCaloClusterEnergy_RecoVsTrue = new TH2F("hCaloClusterEnergy_RecoVsTrue", "Energy (all clusters, Reco vs true)", 110,0,110, 110,0,110);
  hCaloClusterEnergy_RecoVsTrue->SetXTitle("Reco Energy [MeV]");
  hCaloClusterEnergy_RecoVsTrue->SetYTitle("True Energy [MeV]");

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the calo clusters from the event and make sure they have both a reco and MC object
    auto calo_clusters = event.GetCaloClusters([](CaloCluster& cluster){ return has_mc_cluster(cluster) && has_reco_cluster(cluster); });

    // Loop through the e_minus tracks
    for (auto& calo_cluster : calo_clusters) {
      // Fill the histogram
      hCaloClusterEnergy_RecoVsTrue->Fill(calo_cluster.calocluster->energyDep_, calo_cluster.caloclustermc->etot);
    }
  }

  // Draw the histogram
  hCaloClusterEnergy_RecoVsTrue->Draw("COLZ");
}
