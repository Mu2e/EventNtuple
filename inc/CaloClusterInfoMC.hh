//
// CaloClusterInfoMC: MC-truth infromation for calorimeter clusters
//
#ifndef CaloClusterInfoMC_HH
#define CaloClusterInfoMC_HH
#include "Offline/MCDataProducts/inc/MCRelationship.hh"
namespace mu2e
{
  struct CaloClusterInfoMC {
    int nhits; // number of hitmc associated with this cluster
    int nsim; // # of sim particles associated with this cluster
    float etot; // total true energy from all particles in this cluster
    float tavg; // average time over all particles
    float eprimary; // primary particle true energy in this cluster
    float tprimary; // primary particle average time
    std::vector<int> simParticleIds; // list of simparticle ids
    std::vector<MCRelationship> simRels; // relationship to the particle that deposited the most energy in the calo cluster
    std::vector<int>  hits_; // vector of branch indices of hits in the cluster
    MCRelationship prel; // relationship of the cluster primary particle to the event primary
    CaloClusterInfoMC() : nsim(0), etot(0.0), tavg(0.0), eprimary(0.0), tprimary(0.0){}

    void reset() { *this = CaloClusterInfoMC(); }
  };
}
#endif
