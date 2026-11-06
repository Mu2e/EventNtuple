//
// TrkCrvXingInfoMC: truth of one reco CRV crossing, parallel to <trk>crvxings
//
#ifndef TrkCrvXingInfoMC_HH
#define TrkCrvXingInfoMC_HH

namespace mu2e
{
  struct TrkCrvXingInfoMC {
    int truexing = -1; // index into <trk>crvtruexingsmc of the true crossing of the same sector, -1 if none (a fake crossing)
    int nbrxing = -1; // for a fake: index into <trk>crvtruexingsmc of the particle's crossing of a coplanar neighbour, -1 if none

    void reset() { *this = TrkCrvXingInfoMC(); }
  };
}
#endif
