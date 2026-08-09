#ifndef SubRun_hh_
#define SubRun_hh_

#include "EventNtuple/inc/SubRunInfo.hh"
#include "EventNtuple/rooutil/inc/BranchUtils.hh"

namespace rooutil {
  struct SubRun {
    explicit SubRun(TChain* ntuple) {
      has_srinfo = CheckForBranch(ntuple, "srinfo", &srinfo);
      has_gen_event_count = CheckForBranch(ntuple, "genEventCount", &genEventCount);
      has_proc_event_count = CheckForBranch(ntuple, "procEventCount", &procEventCount);
      has_cosmic_livetime = CheckForBranch(ntuple, "cosmicLivetime", &cosmicLivetime);
    }

    mu2e::SubRunInfo* srinfo = nullptr;
    long genEventCount = 0;
    long procEventCount = 0;
    float cosmicLivetime = 0.0;

    bool has_srinfo = false;
    bool has_gen_event_count = false;
    bool has_proc_event_count = false;
    bool has_cosmic_livetime = false;
  };
} // namespace rooutil
#endif
