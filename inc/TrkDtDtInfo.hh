//
// TrkDtDtInfo: Result of track dt_hit / dt_trk fit
// Michael MacKenzie
//
#ifndef TrkDtDtInfo_HH
#define TrkDtDtInfo_HH

namespace mu2e
{
  struct TrkDtDtInfo {
    float slope = 0.; // linear fit slope
    float offset = 0.; // linear fit intercept
    float slopeUnc = 0.; // uncertainty on the slope
    float chisq = 0.; // linear fit chi-squared
    int   dof = 0; // degrees of freedom in linear fit (N(hits) - 2)

    void reset() { *this = TrkDtDtInfo(); }
  };
}
#endif
