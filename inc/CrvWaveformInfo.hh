#ifndef CrvWaveformInfo_hh
#define CrvWaveformInfo_hh

#include <vector>

namespace mu2e
{
  struct CrvWaveformInfo   //information about CRV waveforms
  {
    float adc = -1;
    float time = -1;
    int   barId = -1;
    int   SiPMId = -1;
    CrvWaveformInfo(){}
    CrvWaveformInfo(float adc, float time, int barId, int SiPMId) :
      adc(adc),
      time(time),
      barId(barId),
      SiPMId(SiPMId)
    {}
  };

  typedef std::vector<CrvWaveformInfo> CrvWaveformInfoCollection;  //this is the reco vector which will be stored in the main TTree

}
#endif
