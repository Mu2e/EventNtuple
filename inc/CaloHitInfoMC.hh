//
// CaloHitInfoMC: MC-truth infromation for calorimeter Hits
//
#ifndef CaloHitInfoMC_HH
#define CaloHitInfoMC_HH
#include "Offline/MCDataProducts/inc/MCRelationship.hh"
namespace mu2e
{
  struct CaloHitInfoMC {

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

    CaloHitInfoMC() : nsim(0), eDep(0.0), eDepG4(0.0), eprimary(0.0), tprimary(0.0), clusterIdx_(-1){}

    void reset() { *this = CaloHitInfoMC(); }
  };
}
#endif
