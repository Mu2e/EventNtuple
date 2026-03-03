//
// An example of how to plot reco vs true
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH2F.h"

using namespace rooutil;
void PlotCaloCluster_SimParticles(std::string filename) {

  // Create the histogram you want to fill
  TH2F* hCaloCluster_RecoEnergyVsSimEnergy = new TH2F("hCaloCluster_RecoEnergyVsSimEnergy", "Reco Energy vs Sim Energies", 110,0,110, 110,0,110);
  hCaloCluster_RecoEnergyVsSimEnergy->SetXTitle("Reco Energy [MeV]");
  hCaloCluster_RecoEnergyVsSimEnergy->SetYTitle("Sim Particle Momentum [MeV/c]");

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

      const auto& mc_particles = calo_cluster.GetMCParticles();
      for (const auto& mc_particle : mc_particles) {
        // Fill the histogram
        hCaloCluster_RecoEnergyVsSimEnergy->Fill(calo_cluster.calocluster->energyDep_, mc_particle.mcsim->mom.R());
      }
    }
  }

  // Draw the histogram
  hCaloCluster_RecoEnergyVsSimEnergy->Draw("COLZ");
}
