#include "EventNtuple/inc/CrvPulseInfoReco.hh"
#include "CLHEP/Vector/ThreeVector.h"
namespace mu2e {
  CrvPulseInfoReco::CrvPulseInfoReco(CLHEP::Hep3Vector ppos, int barId, int sectorId, int SiPMId, int PEs, int PEsPulseHeight, float pulseHeight, float pulseBeta, float pulseFitChi2, float time, int crvHitIndex) :
    pos(ppos),
    barId(barId),
    sectorId(sectorId),
    SiPMId(SiPMId),
    PEs(PEs),
    PEsPulseHeight(PEsPulseHeight),
    pulseHeight(pulseHeight),
    pulseBeta(pulseBeta),
    pulseFitChi2(pulseFitChi2),
    time(time),
    crvHitIndex(crvHitIndex)
  {}
}
