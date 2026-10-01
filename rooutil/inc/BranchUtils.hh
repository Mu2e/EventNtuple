#ifndef BranchUtils_hh_
#define BranchUtils_hh_

#include <string>
#include <vector>

#include "TChain.h"
#include "TBranchElement.h"

namespace rooutil {
  inline bool CheckForBranch(TChain* ntuple, const char* branch_name, void* address = nullptr) {
    if (ntuple->GetBranch(branch_name) == nullptr || ntuple->GetBranchStatus(branch_name) == 0) return false;
    if (address != nullptr) ntuple->SetBranchAddress(branch_name, address);
    return true;
  }

  // Find all top-level branches whose stored class matches class_name (e.g.
  // "vector<mu2e::LineSeedInfo>"). (Confirmed against a real ntuple: TBranchElement::GetName()
  // for these split-object vector branches is the plain configured name with no trailing "." --
  // the "." only shows up in GetTitle() -- but strip one anyway in case some ROOT/branch
  // configuration does return it, since a stray dot would silently break SetBranchAddress/
  // GetBranchStatus lookups by name.) Branches that have been disabled via
  // SetBranchStatus(name, 0) are skipped, so a caller can opt a collection out of discovery the
  // same way CheckForBranch already respects that flag.
  inline std::vector<std::string> FindBranchesOfClass(TChain* ntuple, const std::string& class_name) {
    std::vector<std::string> found;
    const auto* branches = ntuple->GetListOfBranches();
    if (branches == nullptr) return found;
    for (const auto& branch_obj : *branches) {
      auto* branch = dynamic_cast<TBranchElement*>(branch_obj);
      if (branch == nullptr) continue;
      if (class_name != branch->GetClassName()) continue;
      std::string name = branch->GetName();
      if (ntuple->GetBranchStatus(name.c_str()) == 0) continue; // respect opt-out
      if (!name.empty() && name.back() == '.') name.pop_back();
      found.emplace_back(name);
    }
    return found;
  }
} // namespace rooutil
#endif
