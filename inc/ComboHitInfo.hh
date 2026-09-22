//
// ComboHitInfo: Basic information about a combo hit associated with a time cluster or a line seed
//
#ifndef EventNtuple_ComboHitInfo_HH
#define EventNtuple_ComboHitInfo_HH

#include "EventNtuple/inc/RootVectors.hh"

namespace mu2e
{
  // named to avoid a dictionary clash with the Offline TrkDiag struct of the same purpose
  struct EventNtupleComboHitInfo {
    int plane = 0, panel = 0, layer = 0, straw = 0; // StrawId fields for the combo hit; for composite hits only the fields common to all components are meaningful
    int nStrawHits = 0; // N(straw hits) combined in this hit
    int nCombo = 0; // N(input hits) directly combined in this hit
    float time = 0.f; // corrected hit time, in ns
    float edep = 0.f; // average straw energy deposition, in MeV
    float qual = 0.f; // quality of the hit or combination
    float wdist = 0.f; // distance from the wire center along the wire direction, in mm
    float wres = -1.f; // position resolution along the wire direction, in mm
    float tres = -1.f; // position resolution transverse to the wire direction, in mm
    XYZVectorF pos = XYZVectorF(0.f, 0.f, 0.f); // position (x, y, z) in mm, in the tracker system
    XYZVectorF udir = XYZVectorF(0.f, 0.f, 0.f); // wire direction unit vector
    void reset() { *this = EventNtupleComboHitInfo(); }
  };
}
#endif
