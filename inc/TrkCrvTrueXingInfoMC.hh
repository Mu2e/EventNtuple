//
// TrkCrvTrueXingInfoMC: one true CRV sector crossing by the track's particle or one of its ancestors
//
#ifndef TrkCrvTrueXingInfoMC_HH
#define TrkCrvTrueXingInfoMC_HH
#include "EventNtuple/inc/RootVectors.hh"

namespace mu2e
{
  struct TrkCrvTrueXingInfoMC {
    int   sid = -1; // SurfaceId of the sector
    int   sector = -1; // CRV sector (CRSScintillatorShield index)
    int   rel = -1; // MCRelationship of the crossing particle to the track's particle: 0 same, 2 mother (its parent), 5 umother (further up)
    int   rem = -1; // generations between them (0 for same), simulation-stage copies not counted
    int   pdg = 0; // PDG code of the crossing particle
    bool  geom = false; // the trajectory crosses the sector mid-plane rectangle (gap-inclusive); pos, dir, time, mom are valid
    XYZVectorF pos; // mid-plane crossing, detector frame (mm)
    XYZVectorF dir; // unit direction of the trajectory at the crossing
    float time = 0; // crossing time (ns)
    float mom = 0; // momentum at the crossing (MeV/c)
    float dedgeu = 0; // distance inside the sector edge along the bars (mm)
    float dedgev = 0; // distance inside the sector edge across the bars (mm)
    bool  hasdeposits = false; // CrvSteps were available, so edep, nlayers and deppos are filled
    float edep = 0; // visible energy deposited by the particle in the sector's bars (MeV)
    int   nlayers = 0; // layers with a deposit from the particle
    XYZVectorF deppos; // energy-weighted position of those deposits, detector frame (mm)
    int   crvcoinc = -1; // index into crvcoincs of the cluster with most energy from the particle in this sector, -1 if none or not in that collection
    int   cand = -1; // index into <trk>crvcands of that cluster, -1 if it is not a candidate
    int   recoxing = -1; // index into <trk>crvxings of the reco crossing of this sector (a projection if not inbounds), -1 if none

    void reset() { *this = TrkCrvTrueXingInfoMC(); }
  };
}
#endif
