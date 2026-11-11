//
// TrkCrvCandInfo: one CRV coincidence cluster associated with a track (candidates ordered geometric by chi2, then time-only)
//
#ifndef TrkCrvCandInfo_HH
#define TrkCrvCandInfo_HH

namespace mu2e
{
  struct TrkCrvCandInfo {
    int   crvcoinc = -1; // index into crvcoincs, -1 if the cluster is not in the collection crvcoincs was filled from
    int   xing = -1; // index into <trk>crvxings of the reference crossing, -1 for a time-only candidate
    int   sector = -1; // dominant CRV sector of the cluster (largest PE sum)
    float dtref = 0; // track reference time - cluster average hit time (ns)
    float dtreftof = 0; // dtref - straight-line flight time between the reference point and the cluster (ns)
    float dtxing = 0; // cluster time - track time at the cluster plane (ns), geometric candidates only
    float du = 0; // cluster - track along the bars at the cluster plane (mm), geometric candidates only
    float dv = 0; // cluster - track across the bars at the cluster plane (mm), geometric candidates only
    float sigu = 0; // track error on du (mm)
    float sigv = 0; // track error on dv (mm)
    float rhouv = 0; // correlation of the track errors on du and dv
    float chi2 = -1; // space + time chi2, -1 for time-only candidates
    int   ndof = 0; // chi2 degrees of freedom (2 single-ended, 3 two-ended)
    int   nshare = 0; // tracks of the same collection holding this cluster as a candidate
    unsigned flags = 0; // TrkCrvCandidate::Flag bits, decoded in the bools below
    bool  geom = false; // kept by the geometric test
    bool  time = false; // kept by the time test
    bool  best = false; // the track's best candidate (bestgeom, else besttime)
    bool  bestforcluster = false; // this track is the cluster's best track
    bool  exclusive = false; // chosen by the one-to-one assignment
    bool  bestinsector = false; // lowest-chi2 geometric candidate of the track in this sector
    bool  samesector = false; // the reference crossing is in the cluster's own sector
    bool  outofbounds = false; // the projected point is outside the cluster's sector rectangle
    bool  singleended = false; // no along-bar measurement (du not in the chi2)
    bool  multisector = false; // the cluster spans sectors
    bool  usedinfit = false; // the cluster is a constraint in this track's fit

    void reset() { *this = TrkCrvCandInfo(); }
  };
}
#endif
