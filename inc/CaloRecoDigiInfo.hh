//
// CaloRecoDigiInfo: calorimeter recodigis with index of raw digi and of parent hit
//
#ifndef CaloRecoDigiInfo_HH
#define CaloRecoDigiInfo_HH

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <vector>
#include "Offline/RecoDataProducts/inc/CaloRecoDigi.hh"

namespace mu2e
{
  struct CaloRecoDigiInfo {

    float               eDep_;             // Fitted energy
    float               eDepErr_;          // Energy fit error
    float               time_;             // Fitted time
    float               timeErr_;          // Time fit error
    float               chi2_;             // Fit chisquare
    unsigned            ndf_;              // Fit NDF
    bool                pileUp_;           // Flag indicating if the hit is affected by pile-up
    int                 caloDigiIdx_;      // Index to the associated CaloDigi

    // not in dataproduct
    int                 caloHitIdx_;       // Hit index
    int                 SiPMID_;           // Offline SiPM ID number 

    CaloRecoDigiInfo() : eDep_(0.0), eDepErr_(0.0), time_(0.0), timeErr_(0.0), chi2_(0.0), ndf_(0), pileUp_(false), caloDigiIdx_(-1), caloHitIdx_(-1), SiPMID_(0) {}
    void reset() { *this = CaloRecoDigiInfo(); }
    
    bool operator== (const CaloRecoDigi& other) const {
      const double eps = 1e-9;
      return std::abs(eDep_    - other.energyDep())    < eps &&
             std::abs(eDepErr_ - other.energyDepErr()) < eps &&
             std::abs(time_    - other.time())    < eps &&
             std::abs(timeErr_ - other.timeErr()) < eps &&
             std::abs(chi2_    - other.chi2())    < eps &&
             ndf_             == other.ndf()            &&
             pileUp_          == other.pileUp();
    }
  };
}
#endif



