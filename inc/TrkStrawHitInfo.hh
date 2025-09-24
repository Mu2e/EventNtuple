//
// TrkStrawHitInfo: information about the straw hits assigned to a track
//
#ifndef TrkStrawHitInfoHH
#define TrkStrawHitInfoHH


//#include "EventNtuple/inc/RootVectors.hh"
#include "Offline/DataProducts/inc/TrkTypes.hh"
namespace mu2e
{
  struct TrkStrawHitInfo {
    int plane = 0, panel = 0, layer = 0, straw = 0;  // StrawId fields for the straw hit
    int state = 0;     // hit state, including activity and left-right ambiguity
    int algo = 0; // updater algorithm last used on this hit
    bool frozen = false; // was state frozen?
    bool usetot, usedriftdt, useabsdt, usendvar; // flags
    float bkgqual = -1.0, signqual = -1.0, driftqual = -1.0, chi2qual = -1.0; // hit state assignment quality
    int earlyend = 0; // which end had the early TDC
    float edep = -1.0 ;        // reco energy deposition
    float wdist = 0;       // raw hit U position
    float werr = -1.0;    // raw hit U position error estimate
    float tottdrift = 0; // TOT expressed as drift time
    TrkTypes::TDCTimes etime = {0,0};   // raw hit time at each end
    TrkTypes::TOTTimes  tot = {0,0};   // TOT times in ns from each end
    float ptoca = 0, stoca = 0;    // reference particle (sensor) time of closest approach (TOCA)
    float rdoca = 0, rdocavar = 0;   // reference (biased) DOCA from the track to the wire, signed by the angular momentum WRT the wire and the measurement end (and variance)
    float rdt = 0, rtocavar = -1.0;   // reference (biased) time difference (and variance) at POCA
    float udoca = 0, udocavar = -1.0; // unbiaed DOCA (and variance)
    float udt = 0, utocavar = -1.0;   // unbiased dt and variance
    float rupos = 0, uupos = 0; // POCA position along the straw WRT the straw middle
    float rdrift = 0, cdrift = 0, sderr = -1.0, uderr = -1.0, dvel = 0, lang = 0; // drift information
    float utresid = 0, utresidmvar = -1.0, utresidpvar = -1.0; // unbiased time residual and associated measurement and parameter variances
    float udresid = 0, udresidmvar = -1.0, udresidpvar = -1.0; // unbiased distance residual and associated measurement and parameter variances
    float ulresid = 0, ulresidmvar = -1.0, ulresidpvar = -1.0; // unbiased longitudinal distance residual and associated measurement and parameter variances
    float rtresid = 0, rtresidmvar = -1.0, rtresidpvar = -1.0; // reference time residual and associated measurement and parameter variances
    float rdresid = 0, rdresidmvar = -1.0, rdresidpvar = -1.0; // reference distance residual and associated measurement and parameter variances
    float rlresid = 0, rlresidmvar = -1.0, rlresidpvar = -1.0; // reference  longiudinal distance residual and associated measurement and parameter variances
    float wdot = 0; // cosine of the angle between the track and wire
    XYZVectorF poca; // POCA
    float uwirephi = 0; // cylindrical phi around wire of uPOCA
    float ustrawdist = 0; // unsigned uDOCA to straw center
    float ustrawphi = 0; // cylindrical phi around straw center of uPOCA
    // not sure if we still want these
    bool dhit = false;
    bool dactive = false;
  };
}
#endif
