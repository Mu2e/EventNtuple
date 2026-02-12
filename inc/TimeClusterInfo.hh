//
// TimeClusterInfo: Information in a reconstructed time cluster
// Michael MacKenzie, 2026
//
#ifndef EventNtuple_TimeClusterInfo_HH
#define EventNtuple_TimeClusterInfo_HH

namespace mu2e
{
  struct EventNtupleTimeClusterInfo {
    int nhits = 0; // N(combo hits)
    int nStrawHits = 0; //N(straw hits)
    float t0 = 0.; // T0 time
    float posX = 0.f; // position
    float posY = 0.f;
    float posZ = 0.f;
    float ecalo = -1.f; // energy of the associated calo cluster
    float tcalo = 0.f; // time of the associated calo cluster
    void reset() { *this = EventNtupleTimeClusterInfo(); }
  };
}
#endif
