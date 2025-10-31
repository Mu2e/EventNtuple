/*
 TriggerInfo: stores information regarding the mu2e triggers passed or failed in the given event
 Sophie Middleton, 2025
*/
#ifndef TriggerInfo_HH
#define TriggerInfo_HH

namespace mu2e
{
  struct TriggerInfo {
    bool DigitizePath = false;
    bool TriggerablePath = false;
    bool mprDe_highP_stopTarg = false;
    bool tprDe_highP_stopTarg = false;
    bool tprDe_highP = false;
    bool tprDe_lowP_stopTarg = false;
    bool tprHelixDe_ipa = false;
    bool tprHelixDe_ipa_phiScaled = false;
    bool tprHelixDe = false;
    bool tprHelixUe = false;
    bool cprDe_highP_stopTarg = false;
    bool cprDe_highP = false;
    bool cprDe_lowP_stopTarg = false;
    bool cprHelixDe = false;
    bool cprHelixUe = false;
    bool apr_highP_stopTarg = false;
    bool apr_highP = false;
    bool apr_lowP_stopTarg = false;
    bool apr_highP_stopTarg_multiTrk = false;
    bool apr_lowP_multiHelix = false;
    bool aprHelix = false;
    bool caloFast_photon = false;
    bool caloFast_MVANNCE = false;
    bool caloFast_cosmic = false;
    bool caloFast_RMC = false;
    bool cstTimeCluster = false;
    bool cstCosmicTrackSeed = false;
    bool minBias_SDCount = false;
    bool minBias_CDCount = false;
    bool caloHitRec_N0Source = false;


    void reset() { *this = TriggerInfo(); }
  };
}
#endif
