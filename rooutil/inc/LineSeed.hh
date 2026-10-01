#ifndef LineSeed_hh_
#define LineSeed_hh_

#include <functional>
#include <vector>

#include "EventNtuple/inc/LineSeedInfo.hh"
#include "EventNtuple/inc/ComboHitInfo.hh"

namespace rooutil {
  struct LineSeed {
    LineSeed(mu2e::LineSeedInfo* ls)
      : lineseed(ls) {
    }

    void Update(bool debug = false) {
    }

    //-------------------------------------------------
    // Accessors

    int        Status()     const { return (lineseed) ? lineseed->status     : 0    ; }
    int        NHits()      const { return (lineseed) ? lineseed->nhits      : 0    ; }
    int        NStrawHits() const { return (lineseed) ? lineseed->nStrawHits : 0    ; }
    float      T0()         const { return (lineseed) ? lineseed->t0         : 0.f  ; }
    float      EDep()       const { return (lineseed) ? lineseed->edep       : -1.f ; }
    float      ECalo()      const { return (lineseed) ? lineseed->ecalo      : -1.f ; }
    float      TCalo()      const { return (lineseed) ? lineseed->tcalo      : 0.f  ; }
    bool       HasCalo()    const { return ECalo() >= 0.f; }

    // The combo hits of this line seed, only filled if the "<collection>hits" branch is in the
    // ntuple (EventNtupleMaker's lineseeds.fillHitsFor)
    bool   HasHits()     const { return hits != nullptr; }
    size_t NComboHits()  const { return (hits) ? hits->size() : 0; }
    const std::vector<mu2e::EventNtupleComboHitInfo>& Hits() const {
      static const std::vector<mu2e::EventNtupleComboHitInfo> empty;
      return (hits) ? *hits : empty;
    }

    // Pointers to the data
    mu2e::LineSeedInfo* lineseed = nullptr;
    std::vector<mu2e::EventNtupleComboHitInfo>* hits = nullptr; // only set if the <collection>hits branch is in the ntuple
  };

  typedef std::function<bool(LineSeed&)> LineSeedCut;
  typedef std::vector<LineSeed> LineSeeds;
} // namespace rooutil
#endif
