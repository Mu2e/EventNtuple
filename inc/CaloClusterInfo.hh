//
// CaloClusterInfo: information for calorimeter clusters
//
#ifndef CaloClusterInfo_HH
#define CaloClusterInfo_HH

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <vector>

namespace mu2e
{
  struct CaloClusterInfo {

    int               diskID_;              //Calo disk number
    float             time_;                //Cluster time
    float             timeErr_;             //Cluster time error
    float             energyDep_;           //Cluster energy
    float             energyDepErr_;        //Cluster energy error
    CLHEP::Hep3Vector cog_;                 //Cluster centroid
    std::vector<int>  hits_;                //vector of branch indices of hits in the cluster
    unsigned          size_;                //Number of hits
    bool              isSplit_;             //Is this cluster split

    CaloClusterInfo() : diskID_(0), time_(0.0), timeErr_(0.0), energyDep_(0.0), energyDepErr_(0.0), cog_(CLHEP::Hep3Vector(0,0,0)), hits_(), size_(0), isSplit_(0) {}
    void reset() { *this = CaloClusterInfo(); }
  };
}
#endif
