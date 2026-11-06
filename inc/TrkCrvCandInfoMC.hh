//
// TrkCrvCandInfoMC: truth of one track-CRV candidate, parallel to <trk>crvcands
//
#ifndef TrkCrvCandInfoMC_HH
#define TrkCrvCandInfoMC_HH

namespace mu2e
{
  struct TrkCrvCandInfoMC {
    bool  hasmc = false; // the cluster has MC information (false: background or no MC)
    int   rel = -1; // closest MCRelationship of the cluster's deposit particles to the track's particle: -1 none, 0 same, 1 daughter, 2 mother, 3 sibling, 4 udaughter, 5 umother, 6 usibling
    int   rem = -1; // generational removal of that relation (0 for same)
    float edep_same = 0; // cluster energy from the track's particle (MeV)
    float edep_related = 0; // cluster energy from any related particle, the track's particle included (MeV)
    float edep_total = 0; // cluster energy with MC information (MeV)

    void reset() { *this = TrkCrvCandInfoMC(); }
  };
}
#endif
