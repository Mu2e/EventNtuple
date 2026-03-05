//
// An example of how to plot reco vs true
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TCanvas.h"
#include "TH2F.h"

using namespace rooutil;

void PlotCaloHits_RecoVsTrue(std::string filename) {

  // Set up RooUtil
  RooUtil util(filename);

  TH2F* hCaloHits_RecoVsTrue = new TH2F("hCaloHits_RecoVsTrue", "Reco vs True (Calo Hits)", 110,0,110, 110,0,110);
  hCaloHits_RecoVsTrue->SetXTitle("Reco EDep [MeV]");
  hCaloHits_RecoVsTrue->SetYTitle("True EDep [MeV]");

  // Loop through the events
  int n_events = util.GetNEvents();
  for (int i_event = 0; i_event < n_events; ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

        // Get the e_minus tracks from the event
    auto calo_clusters = event.GetCaloClusters();

    // Loop through the calo clusters
    int i_cluster = 0;
    for (auto& calo_cluster : calo_clusters) {

      // Get the calohits for this cluster
      auto calohits = calo_cluster.GetHits([](CaloHit& hit){ return has_mc_hit(hit) && has_reco_hit(hit); });

      // Loop through the calo hits
      for (auto& calohit : calohits) {
        hCaloHits_RecoVsTrue->Fill(calohit.reco->eDep_, calohit.mc->eDep);
      }
    }
  }

  TCanvas* c1 = new TCanvas();
  hCaloHits_RecoVsTrue->Draw("COLZ");
}
