#ifndef CaloCluster_hh_
#define CaloCluster_hh_

#include <functional>
#include "EventNtuple/inc/CaloClusterInfo.hh"
#include "EventNtuple/inc/CaloHitInfo.hh"
#include "EventNtuple/inc/CaloClusterInfoMC.hh"

#include "EventNtuple/rooutil/inc/CaloHit.hh"

namespace rooutil {
  struct CaloCluster {
    CaloCluster(mu2e::CaloClusterInfo* calocluster)
      : calocluster(calocluster) {

    }

    void Update(bool debug = false) {
    }

    int nHits() const { return hits.size(); }
    CaloHits GetHits() { return hits; }
    CaloHits GetHits(CaloHitCut cut) {
      CaloHits select_hits;
      for (auto& hit : hits) {
        if (cut(hit)) {
          select_hits.emplace_back(hit);
        }
      }
      return select_hits;
    }
    CaloHits hits;

    int nMCParticles() const { return mc_particles.size();  }
    MCParticles GetMCParticles() { return mc_particles; }
    MCParticles GetMCParticles(MCParticleCut cut) {
      MCParticles select_mc_particles;
      for (auto& mc_particle : mc_particles) {
        if (cut(mc_particle)) {
          select_mc_particles.emplace_back(mc_particle);
        }
      }
      return select_mc_particles;
    }
    MCParticles mc_particles;



    // Pointers to the data
    mu2e::CaloClusterInfo* calocluster = nullptr;
    mu2e::CaloClusterInfoMC* caloclustermc = nullptr;
  };

  typedef std::function<bool(CaloCluster&)> CaloClusterCut;
  typedef std::vector<CaloCluster> CaloClusters;
} // namespace rooutil
#endif
