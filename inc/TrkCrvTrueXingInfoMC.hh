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
    int   generation = 0; // 0 the track's particle, 1 its parent, ...
    int   pdg = 0; // PDG code of the crossing particle
    bool  geom = false; // the trajectory crosses the sector mid-plane rectangle (gap-inclusive); pos, dir, time, mom are valid
    XYZVectorF pos; // mid-plane crossing, detector frame (mm)
    XYZVectorF dir; // unit direction of the trajectory at the crossing
    float time = 0; // crossing time (ns)
    float mom = 0; // momentum at the crossing (MeV/c)
    float dedgeu = 0; // distance inside the sector edge along the bars (mm)
    float dedgev = 0; // distance inside the sector edge across the bars (mm)
    float edep = 0; // visible energy deposited by the particle in the sector's bars (MeV)
    int   nlayers = 0; // layers with a deposit from the particle
    XYZVectorF deppos; // energy-weighted position of those deposits, detector frame (mm)
    int   crvcoinc = -1; // index into crvcoincs of the cluster with most energy from the particle in this sector, -1 if none or not in that collection
    int   cand = -1; // index into <trk>crvcands of that cluster, -1 if it is not a candidate
    int   recoxing = -1; // index into <trk>crvxings of the reco crossing of this sector, -1 if not extrapolated there

    void reset() { *this = TrkCrvTrueXingInfoMC(); }
  };
}
#endif
