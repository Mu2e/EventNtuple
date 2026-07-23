//
// TrkDtDtInfo: Result of track dt_hit / dt_trk fit
// Michael MacKenzie
//
#ifndef TrkDtDtInfo_HH
#define TrkDtDtInfo_HH

namespace mu2e
{
  struct TrkDtDtInfo {
    float slope_ = 0.; // linear fit slope
    float offset_ = 0.; // linear fit intercept
    float slopeUnc_ = 0.; // uncertainty on the slope
    float chisq_ = 0.; // linear fit chi-squared
    int   dof_ = 0; // degrees of freedom in linear fit (N(hits) - 2)

    void reset() { *this = TrkDtDtInfo(); }
  };
}
#endif
