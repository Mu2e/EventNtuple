//
// CaloHitInfoMC: MC-truth infromation for calorimeter Hits
//
#ifndef CaloHitInfoMC_HH
#define CaloHitInfoMC_HH
#include "Offline/MCDataProducts/inc/MCRelationship.hh"
#include "Offline/MCDataProducts/inc/CaloHitMC.hh"

namespace mu2e
{
  struct CaloHitInfoMC {

    int crystalID_; // Crystal ID of the hit
    int nsim; // # of sim particles associated with this hit
    float eDep; // total (corrected) deposited energy in the hit
    float eDepG4; // total G4 deposited energy in the hit
    float eprimary; // energy of the most energetic deposit
    float tprimary; // time of the most energetic deposit
    std::vector<float> eDeps; // list of deposited energies
    std::vector<float> tDeps; // list of times of energy deposits
    std::vector<float> momentumIns; // list of the momentum of the SimParticle when entering in the disk
    std::vector<int> simParticleIds; // list of simparticle ids
    std::vector<MCRelationship> simRels; // relationship to the particle that deposited the most energy in the calo Hit
    int clusterIdx_; // Cluster index

    CaloHitInfoMC() : crystalID_(0), nsim(0), eDep(0.0), eDepG4(0.0), eprimary(0.0), tprimary(0.0), clusterIdx_(-1){}

    void reset() { *this = CaloHitInfoMC(); }

    bool operator== (const CaloHitMC& other) const {
      const double eps = 1e-9;
      auto const& edeps = other.energyDeposits();
      bool same1 = crystalID_     == other.crystalID()             &&
                   nsim           == int(edeps.size())             &&
                   std::abs(eDep   - other.totalEnergyDep()) < eps &&
                   std::abs(eDepG4 - other.totalEnergyDepG4()) < eps;
      if (nsim > 0){ //Only compare first edep if exists
        return same1                                                &&
               std::abs(eprimary - edeps.front().energyDep()) < eps &&
               std::abs(tprimary - edeps.front().time()) < eps;
      } else {
        return same1;
      }
    }
  };
}
#endif
