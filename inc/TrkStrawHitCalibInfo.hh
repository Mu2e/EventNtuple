//
// TrkStrawHitCalibInfo: calib and alignment information about the straw hits assigned to a track
//
#ifndef TrkStrawHitCalibInfoHH
#define TrkStrawHitCalibInfoHH


//#include "EventNtuple/inc/RootVectors.hh"
#include "Offline/DataProducts/inc/TrkTypes.hh"
namespace mu2e
{
  struct TrkStrawHitCalibInfo {
    XYZVectorF dDdX = XYZVectorF(); // derivative of drift distance residual wrt global xyz coordinates
    std::array<float, 6> dDdPlane = {0,0,0,0,0,0}; // derivative of drift distance residual wrt plane local xyz translation/rotations
    std::array<float, 6> dDdPanel = {0,0,0,0,0,0}; // derivative of drift distance residual wrt panel local uvw translation/rotations
    std::array<float, 6> dDdP = {0,0,0,0,0,0}; // derivative of drift distance residual wrt kinkal track parameters
    std::array<float, 6> dLdP = {0,0,0,0,0,0}; // derivative of longitudinal distance residual wrt kinkal track parameters
  };
}
#endif
