//
// TrkCrvCandInfoMC: truth of one track-CRV candidate, parallel to <trk>crvcands
//
#ifndef TrkCrvCandInfoMC_HH
#define TrkCrvCandInfoMC_HH

namespace mu2e
{
  struct TrkCrvCandInfoMC {
    int   rel = 5; // relation of the cluster's deposits to the track's particle: 0 same particle, 1 same family, 2 same primary, 3 unrelated, 4 background, 5 no MC
    float edep_same = 0; // cluster energy from the track's particle and its descendants (MeV)
    float edep_family = 0; // cluster energy from the particle's ancestors and their other descendants (MeV)
    float edep_total = 0; // cluster energy with MC information (MeV)

    void reset() { *this = TrkCrvCandInfoMC(); }
  };
}
#endif
