//
// MCStepInfo: information about StepPointMCs
//
#ifndef MCStepInfo_HH
#define MCStepInfo_HH
#include <vector>
#include "EventNtuple/inc/RootVectors.hh"

namespace mu2e {
  struct MCStepInfo {
    int vid=-1; // volume Id or VirtualDetector Id of this step
    int sid=-1; // matching IntersectionInfo SurfaceId
    int iinter=-1; // matching IntersectionInfo index
    float time = 0;  // time of this step WRT MC primary proton (ns)
    float de = 0; // energy deposit through this step (MeV)
    float dp = 0; // momentum magnitude change throw this step (MeV/c)
    bool early = false;
    bool late = false; // flag if this is the earliest or latest step
    XYZVectorF mom; // particle momentum at the start of this step
    XYZVectorF pos;  // particle position at the start of this step
    void reset() {*this = MCStepInfo(); }
    bool valid() { return vid>=0; }
    int pdg = -1; // true PDG code
    int startCode = -1; // creation process code
    int stopCode = -1; // stop process code

  };

  using MCStepInfos = std::vector<MCStepInfo>;
}
#endif
