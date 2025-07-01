//
// CaloDigiInfo: calorimeter raw digis with index of parent recodigi if any
//
#ifndef CaloDigiInfo_HH
#define CaloDigiInfo_HH

#include <vector>

namespace mu2e
{
  struct CaloDigiInfo {

    int                 SiPMID_;           // Offline SiPM ID number
    int                 t0_;               // Time of first waveform sample
    std::vector<int>    waveform_;         // Waveform
    int                 peakpos_;          // Waveform index of peak
    int                 caloRecoDigiIdx_;  // RecoDigi index

    CaloDigiInfo() : SiPMID_(-1), t0_(0), waveform_(), peakpos_(0), caloRecoDigiIdx_(-1) {}
    void reset() { *this = CaloDigiInfo(); }
  };
}
#endif



