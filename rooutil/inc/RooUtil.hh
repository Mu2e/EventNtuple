#ifndef RooUtil_hh_
#define RooUtil_hh_

#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>

#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"

#include "EventNtuple/rooutil/inc/Event.hh"
#include "EventNtuple/rooutil/inc/SubRun.hh"

namespace rooutil {
  class RooUtil {
  public:
    RooUtil(std::string filename, bool debug = false, std::string treename = "EventNtuple/ntuple") : debug(debug) {
      ntuple = new TChain(treename.c_str());
      subrun_ntuple = new TChain("EventNtuple/subrunNtuple");

      // Check if the given filename contains .root at the end
      std::string root_suffix = ".root";
      if (filename.size() >= root_suffix.size() && filename.compare(filename.size() - root_suffix.size(), root_suffix.size(), root_suffix) == 0) {
        AddFile(filename);
        SetVersionNumber(filename);
      }
      else { //  assume its a file list
        std::ifstream filelist(filename);

        if (filelist.is_open()) {
          std::string line;
          bool first_line = true;
          while (std::getline(filelist, line)) {
            if (line.empty()) continue;
            AddFile(line);

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
      subrun = new SubRun(subrun_ntuple);
    }

    void Debug(bool dbg) { debug = dbg; }

    void SetVersionNumber(std::string filename) {
      TFile* file = new TFile(filename.c_str(), "READ");
      TH1I* version = (TH1I*) file->Get("EventNtuple/version");
      if (!version) {
        std::cout << "Warning: this EventNtuple file does not contain a version number. It is either v06_02_00 or older. This is just a warning..." << std::endl;
      }
      else {
        hVersion = (TH1I*) version->Clone();
        hVersion->SetDirectory(0);
        int majorVer = hVersion->GetBinContent(1);
        int minorVer = hVersion->GetBinContent(2);
        int patchVer = hVersion->GetBinContent(3);
        std::cout << "EventNtuple v" << std::setw(2) << std::setfill('0') << majorVer << "_"
                  << std::setw(2) << std::setfill('0') << minorVer << "_"
                  << std::setw(2) << std::setfill('0') << patchVer << std::endl;
      }
      file->Close();
      delete file;
    }

    void AddFile(const std::string& filename) {
      ntuple->Add(filename.c_str());
      TFile file(filename.c_str(), "READ");
      if (!file.IsZombie() && file.Get("EventNtuple/subrunNtuple") != nullptr) {
        subrun_ntuple->Add(filename.c_str());
      }
      SetTotals(filename);
    }

    void SetTotals(const std::string& filename) {
      TFile* file = new TFile(filename.c_str(), "READ");
      for (const auto& total_name : TotalNames()) {
        TH1* histogram = dynamic_cast<TH1*>(file->Get(("EventNtuple/" + total_name).c_str()));
        if (histogram == nullptr) {
          incomplete_totals.insert(total_name);
          continue;
        }
        totals[total_name] += histogram->GetBinContent(1);
      }
      file->Close();
      delete file;
    }

    int GetNEvents() { return ntuple->GetEntries(); }
    int GetNProcEvents() {
      const auto total = GetTotal("n_proc_events");
      return total ? static_cast<int>(*total) : -1;
    }

    std::optional<double> GetTotal(const std::string& total_name) const {
      if (incomplete_totals.count(total_name) != 0) return std::nullopt;
      const auto total = totals.find(total_name);
      if (total == totals.end()) return std::nullopt;
      return total->second;
    }

    bool HasTotal(const std::string& total_name) const {
      return GetTotal(total_name).has_value();
    }

    std::vector<std::string> GetAvailableTotals() const {
      std::vector<std::string> available_totals;
      for (const auto& total_name : TotalNames()) {
        if (HasTotal(total_name)) available_totals.push_back(total_name);
      }
      return available_totals;
    }

    std::optional<double> GetGeneratedEvents() const { return GetTotal("n_gen_events"); }
    std::optional<double> GetProcessedEvents() const { return GetTotal("n_proc_events"); }
    std::optional<double> GetCosmicLivetime() const { return GetTotal("cosmic_livetime"); }

    std::optional<double> GetRate(double count) const {
      const auto livetime = GetCosmicLivetime();
      if (!livetime || *livetime == 0.0) return std::nullopt;
      return count / *livetime;
    }

    Event& GetEvent(int i_event) {
      if (debug) { std::cout << "RooUtil::GetEvent(): Getting event " << i_event << std::endl; }
      ntuple->GetEntry(i_event);

      if (debug) { std::cout << "RooUtil::GetEvent(): Updating event " << i_event << std::endl; }
      event->Update(debug);

      if (debug) { std::cout << "RooUtil::GetEvent(): Returning event " << i_event << std::endl; }
      return *event;
    }

    int GetNSubRuns() { return subrun_ntuple->GetEntries(); }

    SubRun& GetSubRun(int i_subrun) {
      if (i_subrun < 0 || i_subrun >= GetNSubRuns()) {
        throw std::out_of_range("RooUtil::GetSubRun(): SubRun index is out of range");
      }
      subrun_ntuple->GetEntry(i_subrun);
      return *subrun;
    }

    SubRun* FindSubRun(int run, int subrun_number) {
      BuildSubRunIndex();
      const auto entry = subrun_indices.find(std::make_pair(run, subrun_number));
      if (entry == subrun_indices.end()) return nullptr;
      return &GetSubRun(entry->second);
    }

    using SubRunCut = bool (*)(const SubRun&);

    std::vector<int> GetSubRunIndices(SubRunCut cut) {
      std::vector<int> selected_subruns;
      for (int i_subrun = 0; i_subrun < GetNSubRuns(); ++i_subrun) {
        if (cut(GetSubRun(i_subrun))) selected_subruns.push_back(i_subrun);
      }
      return selected_subruns;
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
    void TurnOffSubRunBranch(const std::string& branchname) {
      subrun_ntuple->SetBranchStatus(branchname.c_str(), 0);
    }
    void TurnOnSubRunBranch(const std::string& branchname) {
      subrun_ntuple->SetBranchStatus(branchname.c_str(), 1);
    }
    void TurnOffAllSubRunBranches() {
      TurnOffSubRunBranch("*");
    }
    void TurnOnAllSubRunBranches() {
      TurnOnSubRunBranch("*");
    }

    void CreateOutputEventNtuple(TFile* outfile) {
      auto dir = outfile->GetDirectory("EventNtuple");
      if (dir == nullptr) dir = outfile->mkdir("EventNtuple");
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
      if(event->trkqual_alt) { output_ntuple->Branch("trkqual_alt", event->trkqual_alt); }
      if(event->trkpid) { output_ntuple->Branch("trkpid", event->trkpid); }
      if(event->trksegs) { output_ntuple->Branch("trksegs", event->trksegs); }
      if(event->trksegsmc) { output_ntuple->Branch("trksegsmc", event->trksegsmc); }
      if(event->trksegpars_lh) { output_ntuple->Branch("trksegpars_lh", event->trksegpars_lh); }
      if(event->trksegpars_ch) { output_ntuple->Branch("trksegpars_ch", event->trksegpars_ch); }
      if(event->trksegpars_kl) { output_ntuple->Branch("trksegpars_kl", event->trksegpars_kl); }
      if(event->trkhits) { output_ntuple->Branch("trkhits", event->trkhits); }
      if(event->trkhitsmc) { output_ntuple->Branch("trkhitsmc", event->trkhitsmc); }
      if(event->trkmats) { output_ntuple->Branch("trkmats", event->trkmats); }
      if(event->trkhitcalibs) { output_ntuple->Branch("trkhitcalibs", event->trkhitcalibs); }

      if(event->timeclusters) { output_ntuple->Branch("timeclusters", event->timeclusters); }

      if(event->caloclusters) { output_ntuple->Branch("caloclusters", event->caloclusters); }
      if(event->calohits) { output_ntuple->Branch("calohits", event->calohits); }
      if(event->calorecodigis) { output_ntuple->Branch("calorecodigis", event->calorecodigis); }
      if(event->calodigis) { output_ntuple->Branch("calodigis", event->calodigis); }
      if(event->caloclustersmc) { output_ntuple->Branch("caloclustersmc", event->caloclustersmc); }
      if(event->calohitsmc) { output_ntuple->Branch("calohitsmc", event->calohitsmc); }

      if(event->crvcoincs) { output_ntuple->Branch("crvcoincs", event->crvcoincs); }
      if(event->crvcoincsmc) { output_ntuple->Branch("crvcoincsmc", event->crvcoincsmc); }
      if(event->crvdigis) { output_ntuple->Branch("crvdigis", event->crvdigis); }
      if(event->crvpulses) { output_ntuple->Branch("crvpulses", event->crvpulses); }
      if(event->crvpulsesmc) { output_ntuple->Branch("crvpulsesmc", event->crvpulsesmc); }
      if(event->crvcoincsmcplane) { output_ntuple->Branch("crvcoincsmcplane", event->crvcoincsmcplane); }

      if(event->trkmcsim) { output_ntuple->Branch("trkmcsim", event->trkmcsim); }
      if(event->calomcsim) { output_ntuple->Branch("calomcsim", event->calomcsim); }

      for (const auto& pair : event->trigger.NameToIndexMap()) {
        output_ntuple->Branch(("trig_" + pair.first).c_str(), &event->triginfo._triggerArray[pair.second]);
      }

      if (event->mcsteps_virtualdetector) { output_ntuple->Branch("mcsteps_virtualdetector", event->mcsteps_virtualdetector); }

      // Write out histograms from input to output
      if (hVersion != nullptr) hVersion->Write();
    }

    void FillOutputEventNtuple() {
      output_ntuple->Fill();
    }

    void CreateOutputSubRunNtuple(TFile* outfile) {
      auto dir = outfile->GetDirectory("EventNtuple");
      if (dir == nullptr) dir = outfile->mkdir("EventNtuple");
      dir->cd();
      output_subrun_ntuple = new TTree("subrunNtuple", "reduced subrun ntuple");

      if (subrun->has_srinfo) { output_subrun_ntuple->Branch("srinfo", subrun->srinfo); }
      if (subrun->has_gen_event_count) { output_subrun_ntuple->Branch("genEventCount", &subrun->genEventCount); }
      if (subrun->has_proc_event_count) { output_subrun_ntuple->Branch("procEventCount", &subrun->procEventCount); }
      if (subrun->has_cosmic_livetime) { output_subrun_ntuple->Branch("cosmicLivetime", &subrun->cosmicLivetime); }
    }

    void FillOutputSubRunNtuple() {
      output_subrun_ntuple->Fill();
    }

  private:
    static const std::vector<std::string>& TotalNames() {
      static const std::vector<std::string> total_names{
        "n_gen_events", "n_proc_events", "cosmic_livetime"
      };
      return total_names;
    }

    void BuildSubRunIndex() {
      if (subrun_index_built) return;
      if (!subrun->has_srinfo) {
        subrun_index_built = true;
        return;
      }
      for (int i_subrun = 0; i_subrun < GetNSubRuns(); ++i_subrun) {
        const auto& current_subrun = GetSubRun(i_subrun);
        subrun_indices[std::make_pair(current_subrun.srinfo->run, current_subrun.srinfo->subrun)] = i_subrun;
      }
      subrun_index_built = true;
    }

    TChain* ntuple;
    Event* event; // holds all the variables for SetBranchAddress
    TChain* subrun_ntuple;
    SubRun* subrun;
    bool debug;

    TH1I* hVersion = nullptr;
    std::map<std::string, double> totals;
    std::set<std::string> incomplete_totals;
    std::map<std::pair<int, int>, int> subrun_indices;
    bool subrun_index_built = false;

    TTree* output_ntuple = nullptr; // for output
    TTree* output_subrun_ntuple = nullptr;
  };
} // namespace rooutil
#endif
