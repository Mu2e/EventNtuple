#ifndef RooUtil_hh_
#define RooUtil_hh_

#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"

#include "EventNtuple/utils/rooutil/inc/Event.hh"

class RooUtil {
public:
  RooUtil(std::string filename, std::string treename = "EventNtuple/ntuple") : debug(false) {
    ntuple = new TChain(treename.c_str());

    // Check if the given filename contains .root at the end
    std::string root_suffix = ".root";
    if (filename.compare(filename.size() - root_suffix.size(), root_suffix.size(), root_suffix) == 0) {
      ntuple->Add(filename.c_str());
      SetVersionNumber(filename);
    }
    else { //  assume its a file list
      std::ifstream filelist(filename);

      if (filelist.is_open()) {
        std::string line;
        bool first_line = true;
        while (std::getline(filelist, line)) {
          ntuple->Add(line.c_str());

          if (first_line) {
            SetVersionNumber(line);
            first_line = false;
          }
        }
        filelist.close();
      } else {
        std::cout << "Error opening filelist." << std::endl;
      }
    }

    event = new Event(ntuple);
  }

  void Debug(bool dbg) { debug = dbg; }

  void SetVersionNumber(std::string filename) {
    TFile* file = new TFile(filename.c_str(), "READ");
    TH1I* hVersion = (TH1I*) file->Get("EventNtuple/version");
    if (!hVersion) {
      std::cout << "Warning: this EventNtuple file does not contain a version number. It is either v06_02_00 or older. This is just a warning..." << std::endl;
    }
    else {
      majorVer = hVersion->GetBinContent(1);
      minorVer = hVersion->GetBinContent(2);
      patchVer = hVersion->GetBinContent(3);
      std::cout << "EventNtuple v" << std::setw(2) << std::setfill('0') << majorVer << "_" << minorVer << "_" << patchVer << std::endl;
    }
    file->Close();
    delete file;
  }

  int GetNEvents() { return ntuple->GetEntries(); }

  Event& GetEvent(int i_event) {
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

  void CreateOutputEventNtuple(TFile* outfile) {
    auto dir = outfile->mkdir("EventNtuple");
    dir->cd();
    output_ntuple = new TTree("ntuple", "reduced ntuple");

    if(event->evtinfo) { output_ntuple->Branch("evtinfo", event->evtinfo); }
    if(event->evtinfomc) { output_ntuple->Branch("evtinfomc", event->evtinfomc); }
    if(event->hitcount) { output_ntuple->Branch("hitcount", event->hitcount); }
    if(event->crvsummary) { output_ntuple->Branch("crvsummary", event->crvsummary); }
    if(event->crvsummarymc) { output_ntuple->Branch("crvsummarymc", event->crvsummarymc); }

    if(event->trk) { output_ntuple->Branch("trk", event->trk); }
    if(event->trkmc) { output_ntuple->Branch("trkmc", event->trkmc); }
    if(event->trkcalohit) { output_ntuple->Branch("trkcalohit", event->trkcalohit); }
    if(event->trkcalohitmc) { output_ntuple->Branch("trkcalohitmc", event->trkcalohitmc); }
    if(event->trkqual) { output_ntuple->Branch("trkqual", event->trkqual); }
    if(event->trksegs) { output_ntuple->Branch("trksegs", event->trksegs); }
    if(event->trksegsmc) { output_ntuple->Branch("trksegsmc", event->trksegsmc); }
    if(event->trksegpars_lh) { output_ntuple->Branch("trksegpars_lh", event->trksegpars_lh); }
    if(event->trksegpars_ch) { output_ntuple->Branch("trksegpars_ch", event->trksegpars_ch); }
    if(event->trksegpars_kl) { output_ntuple->Branch("trksegpars_kl", event->trksegpars_kl); }
    if(event->trkhits) { output_ntuple->Branch("trkhits", event->trkhits); }
    if(event->trkhitsmc) { output_ntuple->Branch("trkhitsmc", event->trkhitsmc); }
    if(event->trkmats) { output_ntuple->Branch("trkmats", event->trkmats); }

    if(event->crvcoincs) { output_ntuple->Branch("crvcoincs", event->crvcoincs); }
    if(event->crvcoincsmc) { output_ntuple->Branch("crvcoincsmc", event->crvcoincsmc); }
    if(event->crvdigis) { output_ntuple->Branch("crvdigis", event->crvdigis); }
    if(event->crvpulses) { output_ntuple->Branch("crvpulses", event->crvpulses); }
    if(event->crvpulsesmc) { output_ntuple->Branch("crvpulsesmc", event->crvpulsesmc); }
    if(event->crvcoincsmcplane) { output_ntuple->Branch("crvcoincsmcplane", event->crvcoincsmcplane); }

    if(event->trkmcsim) { output_ntuple->Branch("trkmcsim", event->trkmcsim); }

  }

  void FillOutputEventNtuple() {
    output_ntuple->Fill();
  }

private:
  TChain* ntuple;
  Event* event; // holds all the variables for SetBranchAddress
  bool debug;

  int majorVer;
  int minorVer;
  int patchVer;

  TTree* output_ntuple; // for output
};

#endif
