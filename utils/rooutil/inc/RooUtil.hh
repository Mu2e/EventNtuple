#ifndef RooUtil_hh_
#define RooUtil_hh_

#include "TFile.h"
#include "TTree.h"

#include "EventNtuple/utils/rooutil/inc/Event.hh"

class RooUtil {
public:
  RooUtil(std::string filename, std::string treename = "EventNtuple/ntuple") : debug(false) {
    TFile* file = new TFile(filename.c_str(), "READ");

    ntuple = (TTree*) file->Get(treename.c_str());

    event = new Event(ntuple);
  }

  void Debug(bool dbg) { debug = dbg; }

  int GetNEvents() { return ntuple->GetEntries(); }

  const Event& GetEvent(int i_event) {
    if (debug) { std::cout << "RooUtil::GetEvent(): Getting event " << i_event << std::endl; }
    ntuple->GetEntry(i_event);

    if (debug) { std::cout << "RooUtil::GetEvent(): Updating event " << i_event << std::endl; }
    event->Update(debug);

    if (debug) { std::cout << "RooUtil::GetEvent(): Returning event " << i_event << std::endl; }
    return *event;
  }

  void TurnOffBranch(const std::string& branchname) {
    ntuple->SetBranchStatus(branchname.c_str(), 0);
  }
  void TurnOnBranch(const std::string& branchname) {
    ntuple->SetBranchStatus(branchname.c_str(), 1);
  }
  void TurnOffBranches(const std::vector<std::string>& branchnames) {
    for (const auto& branchname : branchnames) {
      TurnOffBranch(branchname);
    }
  }
  void TurnOnBranches(const std::vector<std::string>& branchnames) {
    for (const auto& branchname : branchnames) {
      TurnOnBranch(branchname);
    }
  }
  void TurnOffAllBranches() {
    TurnOffBranch("*");
  }
  void TurnOnAllBranches() {
    TurnOnBranch("*");
  }


private:
  TTree* ntuple;
  Event* event; // holds all the variables for SetBranchAddress
  bool debug;
};

#endif
