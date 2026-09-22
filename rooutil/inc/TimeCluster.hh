#ifndef EventNtuple_rooutil_TimeCluster_hh_
#define EventNtuple_rooutil_TimeCluster_hh_

#include <functional>
#include "EventNtuple/inc/TimeClusterInfo.hh"
#include "EventNtuple/inc/ComboHitInfo.hh"

namespace rooutil {
  struct TimeCluster {
    TimeCluster(mu2e::EventNtupleTimeClusterInfo* tc)
      : timecluster(tc) {
    }

    void Update(bool debug = false) {
    }

    // Pointers to the data
    mu2e::EventNtupleTimeClusterInfo* timecluster = nullptr;
    std::vector<mu2e::EventNtupleComboHitInfo>* hits = nullptr; // only set if the timeclustershits branch is in the ntuple
  };

  typedef std::function<bool(TimeCluster&)> TimeClusterCut;
  typedef std::vector<TimeCluster> TimeClusters;
} // namespace rooutil
#endif
