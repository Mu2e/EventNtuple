//
// KalSeedGroup: groups the fits in a KalSeedPtrCollection that share the same set of hits
// (e.g. the De, Ue, Dmu and Umu fits of one track) and ranks the hypotheses within each group.
// One entry per input KalSeedPtr, in the same order, produced by GroupSameTracks.
//
#ifndef KalSeedGroup_HH
#define KalSeedGroup_HH

#include "Offline/RecoDataProducts/inc/KalSeed.hh"

#include <vector>

namespace mu2e {
  struct KalSeedGroup {
    KalSeedPtr kseed; // the fit this entry describes
    int id = -1; // index of the group of fits sharing the same hits, unique within the event
    float score = -1; // raw score used for ranking (e.g. fit consistency)
    float rank = -1; // score normalised within the group (sums to 1 over the group; highest = preferred hypothesis)
  };
  typedef std::vector<mu2e::KalSeedGroup> KalSeedGroupCollection;
}
#endif
