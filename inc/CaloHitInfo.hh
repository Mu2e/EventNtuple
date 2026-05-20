//
// CaloHitInfo: calorimeter hits with indices of recodigis and of parent cluster
//
#ifndef CaloHitInfo_HH
#define CaloHitInfo_HH

#include <vector>
#include "EventNtuple/inc/RootVectors.hh"
#include "Offline/RecoDataProducts/inc/CaloHit.hh"

namespace mu2e
{
  struct CaloHitInfo {

    int               crystalId_;           // Crystal ID
    int               nSiPMs_;              // Number of SiPMs [1,2]
    float             time_;                // Hit time
    float             timeErr_;             // Hit time error
    float             eDep_;                // Hit energy
    float             eDepErr_;             // Hit energy error
    std::vector<int>  recoDigis_;           // vector of branch indices of reco digis

    // not in dataproduct
    int               clusterIdx_;          // Cluster index
    XYZVectorF        crystalPos_;          // Crystal position

    CaloHitInfo() : crystalId_(0), nSiPMs_(0), time_(0.0), timeErr_(0.0), eDep_(0.0), eDepErr_(0.0), recoDigis_(), clusterIdx_(-1), crystalPos_() {}
    void reset() { *this = CaloHitInfo(); }

    bool operator== (const CaloHit& other) const {
      const double eps = 1e-9;
      return crystalId_       == other.crystalID()        &&
             nSiPMs_          == other.nSiPMs()           &&
             std::abs(time_    - other.time()) < eps      &&
             std::abs(timeErr_ - other.timeErr()) < eps   &&
             std::abs(eDep_    - other.energyDep()) < eps &&
             std::abs(eDepErr_ - other.energyDepErr()) < eps;
    }
  };
}
#endif



