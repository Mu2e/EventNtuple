//
// HelixInfo: struct for HelixSeed info
//
#ifndef EventNtuple_HelixInfo_hh
#define EventNtuple_HelixInfo_hh
namespace mu2e {
  struct HelixInfo {
    int nch = -1; // number of combo hits potentially used for this helix
    int ncha = -1; // number of combo hits that are not "outliers"
    int nsh = -1; // number of straw hits in the combo hits potentially used for this helix
    int nsha = -1; // number of straw hits in the combo hits that are not "outliers"
    int flag = -1; // flag for algorithm used (TPR = 0, CPR = 1, APR = 2, MPR = 3)
    float t0 = -1; // time at the tracker center
    float t0err = -1; // uncertainty on time
    float mom = -1; // momentum
    float radius = -1; // helix radius (mm)
    float lambda = -1; // helix dz/dphi (mm/radian)
    float rcent = -1; // radius of the helix center in the XY plane (mm)
    float fcent = -1; // azimuth of the helix center in the XY plane
    float fz0 = -1; // azimuth at z = 0
    float chi2xy = -1; // squared sum of the hit residuals wrt the XY plane normalized by the degree of freedom
    float chi2fz = -1; // squared sum of the hit residuals wrt the ZPhi plane normalized by the degree of freedom
    float ecalo = -1; // energy deposited in calo cluster, -1 if none
    void reset() { *this = HelixInfo(); }
  };
}
#endif
