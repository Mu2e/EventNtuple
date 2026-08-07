#ifndef RooUtil_hh_
#define RooUtil_hh_

#include <algorithm>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>

#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"

#include "EventNtuple/inc/TrkQualMetadata.hh"
#include "EventNtuple/rooutil/inc/Event.hh"
#include "EventNtuple/rooutil/inc/UserBranch.hh"
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
        LoadTrkQualMetadata(filename);
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
            LoadTrkQualMetadata(line);
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

    bool HasTrkQualMetadata(const std::string& output_branch) const {
      return trkqual_metadata.find(output_branch) != trkqual_metadata.end();
    }

    const mu2e::TrkQualMetadata& GetTrkQualMetadata(const std::string& output_branch) const {
      const auto metadata = trkqual_metadata.find(output_branch);
      if (metadata == trkqual_metadata.end()) {
        throw std::runtime_error(
          "No TrkQual metadata is available for output branch " + output_branch);
      }
      return metadata->second;
    }

    void RequireTrkQualVersion(
        const std::string& output_branch, const std::string& expected_model_version) const {
      const auto& metadata = GetTrkQualMetadata(output_branch);
      if (metadata.model_version != expected_model_version) {
        throw std::runtime_error(
          "Unexpected TrkQual version for " + output_branch + ": expected " +
          expected_model_version + ", got " + metadata.model_version);
      }
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

    void SetUserBranches(const std::vector<std::shared_ptr<UserBranchBase>>& branches) {
      for (const auto& branch : branches) {
        branch->Bind(ntuple);
        const auto existing = std::find_if(user_branches.begin(), user_branches.end(),
          [&branch](const std::shared_ptr<UserBranchBase>& registered) {
            return registered->name() == branch->name();
          });
        if (existing == user_branches.end()) {
          user_branches.push_back(branch);
        } else {
          *existing = branch;
        }
      }
      event->SetUserBranches(user_branches);
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
      for (const auto& branch : user_branches) {
        if (branch->is_bound()) {
          branch->BranchOutput(output_ntuple);
        }
      }

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
    void LoadTrkQualMetadata(const std::string& filename) {
      TFile file(filename.c_str(), "READ");
      TH1I* metadata_histogram = nullptr;
      file.GetObject("EventNtuple/trkqual_metadata", metadata_histogram);
      if (metadata_histogram == nullptr) {
        return;
      }

      const std::string input_tag_marker = ": input tag = ";
      const std::string model_version_marker = "; model version = ";
      for (int bin = 1; bin <= metadata_histogram->GetNbinsX(); ++bin) {
        const std::string label = metadata_histogram->GetXaxis()->GetBinLabel(bin);
        const auto input_tag_pos = label.find(input_tag_marker);
        const auto model_version_pos = label.find(model_version_marker);
        if (input_tag_pos == std::string::npos || model_version_pos == std::string::npos ||
            input_tag_pos >= model_version_pos) {
          throw std::runtime_error("Invalid TrkQual metadata in " + filename + ": " + label);
        }

        mu2e::TrkQualMetadata metadata{
          label.substr(0, input_tag_pos),
          label.substr(input_tag_pos + input_tag_marker.size(),
                       model_version_pos - input_tag_pos - input_tag_marker.size()),
          label.substr(model_version_pos + model_version_marker.size())
        };
        const auto existing = trkqual_metadata.find(metadata.output_branch);
        if (existing != trkqual_metadata.end() &&
            (existing->second.input_tag != metadata.input_tag ||
             existing->second.model_version != metadata.model_version)) {
          throw std::runtime_error(
            "TrkQual metadata for " + metadata.output_branch +
            " differs between input files");
        }
        trkqual_metadata[metadata.output_branch] = metadata;
      }
    }

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
    std::map<std::string, mu2e::TrkQualMetadata> trkqual_metadata;
    std::vector<std::shared_ptr<UserBranchBase>> user_branches;

    std::map<std::string, double> totals;
    std::set<std::string> incomplete_totals;
    std::map<std::pair<int, int>, int> subrun_indices;
    bool subrun_index_built = false;

    TTree* output_ntuple = nullptr; // for output
    TTree* output_subrun_ntuple = nullptr;
  };
} // namespace rooutil
#endif
