//
// TrkCrvXingInfo: one CRV sector crossing of a track's extrapolation, or, for a straight-line track that crosses no
// sector, its projection onto a sector's extended plane (inbounds false); with the layer probe of that sector
//
#ifndef TrkCrvXingInfo_HH
#define TrkCrvXingInfo_HH
#include "Offline/DataProducts/inc/CRVId.hh"
#include "EventNtuple/inc/RootVectors.hh"
#include <array>
#include <cmath>

namespace mu2e
{
  struct TrkCrvXingInfo {
    int   sid = -1; // SurfaceId of the crossed CRV sector
    int   sector = -1; // CRV sector (CRSScintillatorShield index)
    XYZVectorF pos; // crossing position on the sector mid-plane, detector frame (mm)
    XYZVectorF dir; // unit track direction at the crossing
    float time = 0; // crossing time (ns)
    float sigu = 0; // track position error along the bars, error model applied (mm)
    float sigv = 0; // track position error across the bars, error model applied (mm)
    float rhouv = 0; // correlation of the track position errors along and across the bars
    float sigt = 0; // track time error (ns)
    float mom = 0; // track momentum at the crossing (MeV/c)
    float momerr = 0; // its error (MeV/c)
    bool  inbounds = false; // crossing inside the sector rectangle (found by the extrapolation); false: a projection, whose candidates run to larger chi2
    float dedgeu = 0; // distance inside the sector edge along the bars (mm), negative outside
    float dedgev = 0; // distance inside the sector edge across the bars (mm), negative outside
    float edgesig = 0; // edge distance in units of the track error
    int   best = -1; // index into <trk>crvcands of the best geometric candidate in this sector, -1 if none
    int   nlayers = 0; // layers probed, 0 if the layer probe did not run
    std::array<int, CRVId::nLayers> probe_bar = {-1, -1, -1, -1}; // predicted bar index per layer, -1 if the line misses the layer
    std::array<int, CRVId::nLayers> probe_dbar = {127, 127, 127, 127}; // bar of the selected pulse - predicted bar, 127 if no pulse in the windows
    std::array<float, CRVId::nLayers> probe_upred = {0, 0, 0, 0}; // predicted position along the bar from its centre (mm)
    std::array<float, CRVId::nLayers> probe_tpred = {0, 0, 0, 0}; // track time at the layer (ns)
    std::array<float, CRVId::nLayers> probe_pe = {0, 0, 0, 0}; // PEs of the selected bar's in-window pulses
    std::array<float, CRVId::nLayers> probe_dt = {0, 0, 0, 0}; // pulse time - predicted pulse time (ns)
    std::array<bool, CRVId::nLayers> probe_incluster = {false, false, false, false}; // the selected pulse belongs to a coincidence cluster
    std::array<float, CRVId::nLayers> probe_utdiv = {NAN, NAN, NAN, NAN}; // position along the bar from the two-end time difference (mm), NaN if one-ended

    void reset() { *this = TrkCrvXingInfo(); }
  };
}
#endif
