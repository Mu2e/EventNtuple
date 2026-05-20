//
// CaloDigiInfo: calorimeter raw digis with index of parent recodigi if any
//
#ifndef CaloDigiInfo_HH
#define CaloDigiInfo_HH

#include <vector>
#include "Offline/RecoDataProducts/inc/CaloDigi.hh"

namespace mu2e
{
  struct CaloDigiInfo {

    int                 SiPMID_;           // Offline SiPM ID number
    int                 t0_;               // Time of first waveform sample
    std::vector<int>    waveform_;         // Waveform
    int                 peakpos_;          // Waveform index of peak

    // not in dataproduct
    int                 diskID_;           // Offline disk ID number
    int                 caloRecoDigiIdx_;  // RecoDigi index
    int                 peakval_;          // Waveform value at peak
    XYZVectorF          crystalPos_;       // Crystal position

    CaloDigiInfo() : SiPMID_(-1), t0_(0), waveform_(), peakpos_(0), diskID_(-1), caloRecoDigiIdx_(-1), peakval_(0), crystalPos_() {}
    void reset() { *this = CaloDigiInfo(); }
    
    bool operator== (const CaloDigi& other) const {
      return SiPMID_          == other.SiPMID()  &&
             t0_              == other.t0()      &&
             peakpos_         == other.peakpos() &&
             waveform_.size() == other.waveform().size(); //no need to check the entire waveform
    }
  };
}
#endif



