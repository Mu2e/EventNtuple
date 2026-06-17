#ifndef CrvHitInfoReco_hh
#define CrvHitInfoReco_hh

// CrvHitInfoReco: information about a cluster of CRV coincidence triplets

#include "EventNtuple/inc/RootVectors.hh"
#include "Offline/DataProducts/inc/CRVId.hh"
namespace CLHEP {class Hep3Vector; }
#include <vector>
#include <array>

namespace mu2e
{

  struct CrvHitInfoReco
  {
    int               sectorType =-1;   //CRV sector type
    bool              hitPosAndTimeCalculated = false;  //indicates if pos and time was calculated, or if it was assumed that longitudinal pos was at the center of the counters
    XYZVectorF          pos; //average position of counters
    float             timeStart = -1; //first hit time
    float             timeEnd = -1;   //last hit time
    float             time = -1; // average hit time
    float             PEs = -1;   //total number of PEs for this cluster
    std::array<float, CRVId::nLayers> PEsPerLayer = {-1};  // PEs per layer for this cluster
    std::array<float, CRVId::nLayers * CRVId::nSidesPerBar> sidePEsPerLayer = {-1};// PEs per layer per side for this cluster
    std::array<size_t, CRVId::nSidesPerBar> sidePulses = {0};// number of reco pulses per side for this cluster
    std::array<float, CRVId::nSidesPerBar> sidePEs = {-1};// PEs per side for this cluster
    std::array<float, CRVId::nSidesPerBar> sideTimes = {-1};// avg time per side for this cluster
    int               nHits = -1;      //number of coincidence hits in this cluster
    int               nLayers = -1;      //number of coincidence layers in this cluster
    float             angle = -999;   //coincidence direction

    CrvHitInfoReco(){}
    CrvHitInfoReco(int sectorType, bool hitPosAndTimeCalculated, CLHEP::Hep3Vector hpos, float timeWindowStart, float timeWindowEnd, float timeAvg, float PEs, std::array<float, CRVId::nLayers> PEsPerLayer, std::array<float, CRVId::nLayers * CRVId::nSidesPerBar> sidePEsPerLayer, std::array<size_t, CRVId::nSidesPerBar> sidePulses, std::array<float, CRVId::nSidesPerBar> sidePEs, std::array<float, CRVId::nSidesPerBar> sideTimes, int nCoincidenceHits, int nCoincidenceLayers, float coincidenceAngle);
  };
  typedef std::vector<CrvHitInfoReco> CrvHitInfoRecoCollection;  //this is the reco vector which will be stored in the main TTree
}
#endif
