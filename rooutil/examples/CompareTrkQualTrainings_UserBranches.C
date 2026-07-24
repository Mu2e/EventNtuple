//
// An example of comparing two track-quality branches using RooUtil's
// typed UserBranch registration instead of hard-coded RooUtil members.
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH2F.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLatex.h"

#include <cmath>
#include <iostream>

using namespace rooutil;

void CompareTrkQualTrainings_UserBranches(std::string filename,
                                          std::string reference_branch = "trkqual",
                                          std::string candidate_branch = "trkqual_candidate") {

  bool save_plots = false;
  std::string plotsdir = "/exp/mu2e/app/users/edmonds/plots/2025-09-25/";

  TH2F* hTrkQual_ReferenceVsCandidate = new TH2F("hTrkQual_ReferenceVsCandidate", "", 100,0,1, 100,0,1);
  TH2F* hTrkQual_ReferenceVsCandidate_HQ = new TH2F("hTrkQual_ReferenceVsCandidate_HQ", "", 100,0,1, 100,0,1);
  TH2F* hTrkQual_ReferenceVsCandidate_LQ = new TH2F("hTrkQual_ReferenceVsCandidate_LQ", "", 100,0,1, 100,0,1);

  RooUtil util(filename);

  auto reference_trkqual = MakeTrackUserBranch<mu2e::MVAResultInfo>(reference_branch);
  auto candidate_trkqual = MakeTrackUserBranch<mu2e::MVAResultInfo>(candidate_branch);
  util.SetUserBranches({reference_trkqual, candidate_trkqual});

  if (!reference_trkqual->is_bound()) {
    std::cout << "Could not bind reference branch " << reference_branch << std::endl;
    return;
  }
  if (!candidate_trkqual->is_bound()) {
    std::cout << "Could not bind candidate branch " << candidate_branch << std::endl;
    return;
  }

  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    auto& event = util.GetEvent(i_event);
    auto e_minus_tracks = event.GetTracks(is_e_minus);

    for (auto& track : e_minus_tracks) {
      auto* reference_trkqual_result = track.GetUserBranch<mu2e::MVAResultInfo>(reference_branch);
      auto* candidate_trkqual_result = track.GetUserBranch<mu2e::MVAResultInfo>(candidate_branch);
      if (reference_trkqual_result == nullptr || candidate_trkqual_result == nullptr) {
        continue;
      }

      const auto reference_value = reference_trkqual_result->result;
      const auto candidate_value = candidate_trkqual_result->result;

      hTrkQual_ReferenceVsCandidate->Fill(reference_value, candidate_value);
      auto trk_ent_segments = track.GetSegments([](TrackSegment& segment){ return tracker_entrance(segment) && has_mc_step(segment) && has_reco_step(segment); });
      for (auto& segment : trk_ent_segments) {
        auto mom_res = segment.trkseg->mom.R() - segment.trksegmc->mom.R();
        if (std::fabs(mom_res) < 0.25) {
          hTrkQual_ReferenceVsCandidate_HQ->Fill(reference_value, candidate_value);
        }
        else if (mom_res > 0.70) {
          hTrkQual_ReferenceVsCandidate_LQ->Fill(reference_value, candidate_value);
        }
      }
    }
  }

  double reference_cut_val = 0.93;
  TLine* reference_cut = new TLine(reference_cut_val, 0, reference_cut_val, 1.0);
  reference_cut->SetLineWidth(2);
  reference_cut->SetLineStyle(kDashed);

  double candidate_cut_val = 0.95;
  TLine* candidate_cut = new TLine(0, candidate_cut_val, 1.0, candidate_cut_val);
  candidate_cut->SetLineWidth(2);
  candidate_cut->SetLineStyle(kDashed);

  auto draw_summary = [&](TCanvas* canvas, TH2F* hist, const std::string& title) {
    canvas->SetLogz();
    hist->SetStats(false);
    hist->SetTitle(title.c_str());
    hist->SetXTitle(reference_branch.c_str());
    hist->SetYTitle(candidate_branch.c_str());
    hist->Draw("COLZ");
    reference_cut->Draw("LSAME");
    candidate_cut->Draw("LSAME");

    int reference_min_bin = hist->GetXaxis()->FindBin(reference_cut_val);
    int candidate_min_bin = hist->GetYaxis()->FindBin(candidate_cut_val);
    auto fails_both = hist->Integral(1, reference_min_bin, 1, candidate_min_bin);
    auto passes_reference_fails_candidate = hist->Integral(reference_min_bin, 100, 1, candidate_min_bin);
    auto fails_reference_passes_candidate = hist->Integral(1, reference_min_bin, candidate_min_bin, 100);
    auto passes_both = hist->Integral(reference_min_bin, 100, candidate_min_bin, 100);

    std::cout << title << std::endl;
    std::cout << "  fails both cuts = " << fails_both << std::endl;
    std::cout << "  passes " << reference_branch << ", fails " << candidate_branch << " = " << passes_reference_fails_candidate << std::endl;
    std::cout << "  fails " << reference_branch << ", passes " << candidate_branch << " = " << fails_reference_passes_candidate << std::endl;
    std::cout << "  passes both cuts = " << passes_both << std::endl;

    TLatex* latex = new TLatex();
    latex->SetTextSize(0.06);
    latex->SetTextAlign(22);
    latex->SetTextColor(kRed);
    latex->DrawLatex(0.4, 0.4, Form("#splitline{%.0f}{fail both}", fails_both));
    latex->DrawLatex(0.4, 1.0, Form("%.0f fail %s, pass %s", fails_reference_passes_candidate, reference_branch.c_str(), candidate_branch.c_str()));
    latex->DrawLatex(0.95, 0.4, Form("%.0f", passes_reference_fails_candidate));
    latex->DrawLatex(1.0, 1.0, Form("%.0f", passes_both));
  };

  TCanvas* c1 = new TCanvas();
  draw_summary(c1, hTrkQual_ReferenceVsCandidate, "All Tracks");

  TCanvas* c2 = new TCanvas();
  draw_summary(c2, hTrkQual_ReferenceVsCandidate_HQ, "True High-Quality Tracks");

  TCanvas* c3 = new TCanvas();
  draw_summary(c3, hTrkQual_ReferenceVsCandidate_LQ, "True Low-Quality Tracks");

  if (save_plots) {
    std::string pngname = plotsdir + "/Mu2eTrkQual_CompareTrkQualTrainings_UserBranches_All.png";
    c1->SaveAs(pngname.c_str());

    pngname = plotsdir + "/Mu2eTrkQual_CompareTrkQualTrainings_UserBranches_HighQual.png";
    c2->SaveAs(pngname.c_str());

    pngname = plotsdir + "/Mu2eTrkQual_CompareTrkQualTrainings_UserBranches_LowQual.png";
    c3->SaveAs(pngname.c_str());
  }
}
