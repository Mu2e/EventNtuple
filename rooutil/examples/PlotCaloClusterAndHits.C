//
// An example of how to plot reco vs true
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH2F.h"

using namespace rooutil;
void PlotCaloClusterAndHits(std::string filename) {

  // Create the histogram you want to fill
  TH2F* hCaloClusterHits = new TH2F("hCaloClusterHits", "Energy (all hits, first event)", 110,0,50, 10,0,10);
  hCaloClusterHits->SetXTitle("Hit Energy [MeV]");
  hCaloClusterHits->SetYTitle("Cluster #");

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  int n_events = 1; // use util.GetNEvents(); to run over all events
  for (int i_event = 0; i_event < n_events; ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto calo_clusters = event.GetCaloClusters();

    // Loop through the calo clusters
    int i_cluster = 0;
    for (auto& calo_cluster : calo_clusters) {

      // Get the calohits for this cluster
      auto calohits = calo_cluster.GetHits();

      // Loop through the calo hits
      for (auto& calohit : calohits) {
        hCaloClusterHits->Fill(i_cluster, calohit.reco->eDep_);
      }
      ++i_cluster;
    }
  }

  // Draw the histogram
  hCaloClusterHits->Draw("COLZ");
}
