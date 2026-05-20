#ifndef CrvPulseInfoReco_hh
#define CrvPulseInfoReco_hh

#include "EventNtuple/inc/RootVectors.hh"
namespace CLHEP {class Hep3Vector; }
#include <vector>

namespace mu2e
{

  struct CrvPulseInfoReco   //information about CRV reco pulses
  {

    XYZVectorF pos;           //average position of counters
    int    barId = -1;           //CRV counter ID
    int    sectorId = -1;        //CRV sector ID
    int    SiPMId = -1;          //SiPMId number
    int    PEs = -1;             //PEs using pulse integral
    int    PEsPulseHeight = -1;  //PEs using pulse height
    float  pulseHeight = -1;     //Pulse height
    float  pulseBeta = -1;       //Pulse beta
    float  pulseFitChi2 = -1;    //Pulse Fit chi2
    float  time = -1;            //Time

    CrvPulseInfoReco(){}
    CrvPulseInfoReco(CLHEP::Hep3Vector ppos, int barId, int sectorId, int SiPMId, int PEs, int PEsPulseHeight, float pulseHeight, float pulseBeta, float pulseFitChi2, float time);
  };

  typedef std::vector<CrvPulseInfoReco> CrvPulseInfoRecoCollection;  //this is the reco vector which will be stored in the main TTree

}
#endif
