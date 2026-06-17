#include "EventNtuple/inc/CrvPulseInfoReco.hh"
#include "CLHEP/Vector/ThreeVector.h"
namespace mu2e {
  CrvPulseInfoReco::CrvPulseInfoReco(CLHEP::Hep3Vector ppos, int barId, int sectorId, int SiPMId, int ROC, int FEB, int FEBchannel, int PEs, int PEsPulseHeight, float pulseHeight, float pulseBeta, float pulseFitChi2, float time, int crvHitIndex) :
    pos(ppos),
    barId(barId),
    sectorId(sectorId),
    SiPMId(SiPMId),
    ROC(ROC),
    FEB(FEB),
    FEBchannel(FEBchannel),
    PEs(PEs),
    PEsPulseHeight(PEsPulseHeight),
    pulseHeight(pulseHeight),
    pulseBeta(pulseBeta),
    pulseFitChi2(pulseFitChi2),
    time(time),
    crvHitIndex(crvHitIndex)
  {}
}
