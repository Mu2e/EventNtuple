//
// LineSeedInfo: Information in a reconstructed line seed (cosmic track seed)
// Michael MacKenzie, 2026
//
#ifndef EventNtuple_LineSeedInfo_HH
#define EventNtuple_LineSeedInfo_HH

#include "EventNtuple/inc/RootVectors.hh"

namespace mu2e
{
  struct LineSeedInfo {
    int status = 0; // TrkFitFlag
    int nhits = 0; // N(combo hits)
    int nStrawHits = 0; //N(straw hits)
    float t0 = 0.; // T0 time
    float d0 = 0.; // Track d0
    float phi0 = 0.; // Track phi0
    float z0 = 0.; // Track z0
    float cos = 0.; // Track cos(theta)
    float A0 = 0.; // Fit parameter A0
    float B0 = 0.; // Fit parameter B0
    float A1 = 0.; // Fit parameter A1
    float B1 = 0.; // Fit parameter B1
    float ecalo = -1.f; // energy of the associated calo cluster
    float tcalo = 0.f; // time of the associated calo cluster
    void reset() { *this = LineSeedInfo(); }
  };
}
#endif
