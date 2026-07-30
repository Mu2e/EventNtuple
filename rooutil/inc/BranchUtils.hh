#ifndef BranchUtils_hh_
#define BranchUtils_hh_

#include "TChain.h"

namespace rooutil {
  inline bool CheckForBranch(TChain* ntuple, const char* branch_name, void* address = nullptr) {
    if (ntuple->GetBranch(branch_name) == nullptr || ntuple->GetBranchStatus(branch_name) == 0) return false;
    if (address != nullptr) ntuple->SetBranchAddress(branch_name, address);
    return true;
  }
} // namespace rooutil
#endif
