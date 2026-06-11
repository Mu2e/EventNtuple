#include "EventNtuple/inc/CrvHitInfoReco.hh"
#include "CLHEP/Vector/ThreeVector.h"
namespace mu2e {
  CrvHitInfoReco::CrvHitInfoReco(int sectorType, bool hitPosAndTimeCalculated, CLHEP::Hep3Vector hpos, float timeWindowStart, float timeWindowEnd, float timeAvg, float PEs, std::array<float, CRVId::nLayers> PEsPerLayer, std::array<float, CRVId::nLayers * CRVId::nSidesPerBar> sidePEsPerLayer, std::array<size_t, CRVId::nSidesPerBar> sidePulses, std::array<float, CRVId::nSidesPerBar> sidePEs, std::array<float, CRVId::nSidesPerBar> sideTimes, int nCoincidenceHits, int nCoincidenceLayers, float coincidenceAngle) :
    sectorType(sectorType),
    hitPosAndTimeCalculated(hitPosAndTimeCalculated),
    pos(hpos),
    timeStart(timeWindowStart),
    timeEnd(timeWindowEnd),
    time(timeAvg),
    PEs(PEs),
    PEsPerLayer(PEsPerLayer),
    sidePEsPerLayer(sidePEsPerLayer),
    sidePulses(sidePulses),
    sidePEs(sidePEs),
    sideTimes(sideTimes),
    nHits(nCoincidenceHits),
    nLayers(nCoincidenceLayers),
    angle(coincidenceAngle)
  {}
}
