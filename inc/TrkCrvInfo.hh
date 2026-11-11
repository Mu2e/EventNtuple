//
// TrkCrvInfo: track-CRV association summary of one track, from Offline TrkCrvMatch
//
#ifndef TrkCrvInfo_HH
#define TrkCrvInfo_HH

namespace mu2e
{
  struct TrkCrvInfo {
    bool  matched = false; // a TrkCrvMatch record exists for this track (false: the other leaves are defaults)
    int   nxings = 0; // CRV sector crossings of the track's extrapolation (entries of <trk>crvxings)
    int   ncands = 0; // associated CRV coincidence clusters (entries of <trk>crvcands)
    int   ngeom = 0; // candidates kept by the geometric test
    int   bestgeom = -1; // index into <trk>crvcands of the lowest-chi2 geometric candidate, -1 if none
    int   besttime = -1; // index into <trk>crvcands of the time candidate closest to the configured dtref centre, -1 if none
    int   refsid = -1; // SurfaceId of the surface that defined tref, -1 if the KalSeed t0 was used
    float tref = 0; // track reference time used by dtref: by default the track's entry into the tracker (ns)
    int   probe = 0; // pulses the layer probe saw: 0 not run, 1 none (predicted bars only), 2 clustered pulses only, 3 all pulses
    float timeoffset = 0; // CRV - tracker time offset the matcher applied to cluster times (ns)

    void reset() { *this = TrkCrvInfo(); }
  };
}
#endif
