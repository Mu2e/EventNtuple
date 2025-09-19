#ifndef CaloCluster_hh_
#define CaloCluster_hh_

#include <functional>
#include "EventNtuple/inc/CaloClusterInfo.hh"
#include "EventNtuple/inc/CaloHitInfo.hh"

struct CaloCluster {
  CaloCluster(mu2e::CaloClusterInfo* calocluster)
    : calocluster(calocluster) {

  }

  void Update(bool debug = false) {
  }

  // Pointers to the data
  mu2e::CaloClusterInfo* calocluster = nullptr;
};

typedef std::function<bool(CaloCluster&)> CaloClusterCut;
typedef std::vector<CaloCluster> CaloClusters;

#endif
