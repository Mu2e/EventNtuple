#ifndef EventNtuple_rooutil_TimeCluster_hh_
#define EventNtuple_rooutil_TimeCluster_hh_

#include <functional>
#include "EventNtuple/inc/TimeClusterInfo.hh"

namespace rooutil {
  struct TimeCluster {
    TimeCluster(mu2e::EventNtupleTimeClusterInfo* tc)
      : timecluster(tc) {
    }

    void Update(bool debug = false) {
    }

    // Pointers to the data
    mu2e::EventNtupleTimeClusterInfo* timecluster = nullptr;
  };

  typedef std::function<bool(TimeCluster&)> TimeClusterCut;
  typedef std::vector<TimeCluster> TimeClusters;
} // namespace rooutil
#endif
