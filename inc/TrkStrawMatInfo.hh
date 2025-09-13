//
// TrkStrawMatInfo: information on the straw materials used in the Kalman fit
//
#ifndef TrkStrawMatInfoHH
#define TrkStrawMatInfoHH
#include "EventNtuple/inc/RootVectors.hh"
namespace mu2e
{
  struct TrkStrawMatInfo {
    bool active = false;    // was this material was used in the Kalman fit or not
    bool hashit = false;    // was there a hit assocated with this straw?
    bool activehit = false;    // was the hit assocated active
    bool drifthit = false;    // was the hit assocated using drift information
    int plane = -1, panel = -1, layer = -1, straw = -1; // StrawId fields of the straw
    int pcalc; // flag for how the path length was calculated
    float doca = -1000.0;    // DOCA between the track fit and the straw axis
    float docavar = -1000.0;    // DOCA variance
    float dp  = -1000.0;      // momentum (energy) loss induced by this straw's material, including both entry and exit wall and the gas
    float radlen = -1000.0;  // radiation length of this straw's material seen by the track (including angular effects)
    float dirdot = -1000.0;  // dot product between straw axis and track direciton
    float gaspath = -1000.0;  // path through the straw gas
    float wallpath = -1000.0;  // path through the straw walls
    float wirepath = -1000.0;  // path through the straw wires
    float upos = 0.0;  // distance of the POCA along the straw WRT the straw middle
    float udist = -1000.0;  // distance of the POCA along the straw past the end of the straw
    XYZVectorF poca; // point of closest approach
  };
}
#endif
