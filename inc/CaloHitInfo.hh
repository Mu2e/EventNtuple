//
// CaloHitInfo: calorimeter hits with indeces of recodigis and of parent cluster
//
#ifndef CaloHitInfo_HH
#define CaloHitInfo_HH

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <vector>

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
    int               clusterIdx_;          // Cluster index

    CaloHitInfo() : crystalId_(0), nSiPMs_(0), time_(0.0), timeErr_(0.0), eDep_(0.0), eDepErr_(0.0), recoDigis_(), clusterIdx_(-1) {}
    void reset() { *this = CaloHitInfo(); }
  };
}
#endif



