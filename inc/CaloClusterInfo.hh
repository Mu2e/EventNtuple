//
// CaloClusterInfo: calorimeter clusters with indices of hits
//
#ifndef CaloClusterInfo_HH
#define CaloClusterInfo_HH

#include <vector>
#include "EventNtuple/inc/RootVectors.hh"

namespace mu2e
{
  struct CaloClusterInfo {

    int               diskID_;              // Calo disk number
    float             time_;                // Cluster time
    float             timeErr_;             // Cluster time error
    float             energyDep_;           // Cluster energy
    float             energyDepErr_;        // Cluster energy error
    XYZVectorF        cog_;                 // Cluster centroid
    std::vector<int>  hits_;                // vector of branch indices of hits in the cluster
    unsigned          size_;                // Number of hits
    bool              isSplit_;             // Is this cluster split
    float             secondMoment_;          // Second moment of cluster distribution (mm²)
    float             e1_;                    // Energy in 1 highest crystal (MeV)
    float             e2_;                    // Energy in 2 highest crystals (MeV)
    float             e9_;                    // Energy in 3x3 crystal neighbors (MeV)
    float             e25_;                   // Energy in 5x5 crystal neighbors (MeV)


    CaloClusterInfo() : diskID_(0), time_(0.0), timeErr_(0.0), energyDep_(0.0), energyDepErr_(0.0), cog_(), hits_(), size_(0), isSplit_(0), secondMoment_(-1), e1_(-1), e2_(-1), e9_(-1), e25_(-1) {}
    void reset() { *this = CaloClusterInfo(); }
  };
}
#endif
