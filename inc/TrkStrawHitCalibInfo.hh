//
// TrkStrawHitInfo: information about the straw hits assigned to a track
//
#ifndef TrkStrawHitCalibInfoHH
#define TrkStrawHitCalibInfoHH


//#include "EventNtuple/inc/RootVectors.hh"
#include "Offline/DataProducts/inc/TrkTypes.hh"
namespace mu2e
{
  struct TrkStrawHitCalibInfo {
    XYZVectorF dDdX;
    std::array<float, 6> dDdPlane;
    std::array<float, 6> dDdPanel;
    std::array<float, 6> dDdP;
    std::array<float, 6> dLdP;
  };
}
#endif
