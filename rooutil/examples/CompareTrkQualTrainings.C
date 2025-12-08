//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH2F.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLatex.h"

using namespace rooutil;

void CompareTrkQualTrainings(std::string filename) {

  bool save_plots = false;
  std::string plotsdir = "/exp/mu2e/app/users/edmonds/plots/2025-09-25/";

  // Create the histogram you want to fill
  TH2F* hTrkQual_OldVsNew = new TH2F("hTrkQual_OldVsNew", "", 100,0,1, 100,0,1);
  TH2F* hTrkQual_OldVsNew_HQ = new TH2F("hTrkQual_OldVsNew_HQ", "", 100,0,1, 100,0,1);
  TH2F* hTrkQual_OldVsNew_LQ = new TH2F("hTrkQual_OldVsNew_LQ", "", 100,0,1, 100,0,1);

  // Set up RooUtil
  RooUtil util(filename);
  //  util.Debug(true);
  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto e_minus_tracks = event.GetTracks(is_e_minus);

    // Loop through the e_minus tracks
    for (auto& track : e_minus_tracks) {

      auto old_trkqual = track.trkqual->result;
      auto new_trkqual = track.trkqual_alt->result;

      hTrkQual_OldVsNew->Fill(old_trkqual, new_trkqual);
      // Get the track segments at the tracker entrance and has an MC step
      auto trk_ent_segments = track.GetSegments([](TrackSegment& segment){ return tracker_entrance(segment) && has_mc_step(segment) && has_reco_step(segment); });

      // Loop through the tracker entrance track segments
      for (auto& segment : trk_ent_segments) {

        auto mom_res = segment.trkseg->mom.R() - segment.trksegmc->mom.R();
        if (std::fabs(mom_res) < 0.25) {
          hTrkQual_OldVsNew_HQ->Fill(old_trkqual, new_trkqual);
        }
        else if (mom_res > 0.70) {
          hTrkQual_OldVsNew_LQ->Fill(old_trkqual, new_trkqual);
        }
      }
    }
  }

  // Lines for cuts
  double old_trkqual_cut_val = 0.93;
  TLine* old_trkqual_cut = new TLine(old_trkqual_cut_val, 0, old_trkqual_cut_val, 1.0);
  old_trkqual_cut->SetLineWidth(2);
  old_trkqual_cut->SetLineStyle(kDashed);

  double new_trkqual_cut_val = 0.95;
  TLine* new_trkqual_cut = new TLine(0, new_trkqual_cut_val, 1.0, new_trkqual_cut_val);
  new_trkqual_cut->SetLineWidth(2);
  new_trkqual_cut->SetLineStyle(kDashed);

  // Draw the histogram
  TCanvas* c1 = new TCanvas();
  c1->SetLogz();
  hTrkQual_OldVsNew->SetStats(false);
  hTrkQual_OldVsNew->SetTitle("All Tracks");
  hTrkQual_OldVsNew->SetXTitle("TrkQual ANN1 v1");
  hTrkQual_OldVsNew->SetYTitle("TrkQual ANN1 v2");
  hTrkQual_OldVsNew->Draw("COLZ");
  old_trkqual_cut->Draw("LSAME");
  new_trkqual_cut->Draw("LSAME");

  int old_trkqual_min_bin = hTrkQual_OldVsNew->GetXaxis()->FindBin(old_trkqual_cut_val);
  int new_trkqual_min_bin = hTrkQual_OldVsNew->GetYaxis()->FindBin(new_trkqual_cut_val);
  //  auto integral_old_trkqual = hTrkQual_OldVsNew->Integral(old_trkqual_min_bin, old_trkqual_max_bin, 1, 100);

  double n_total_tracks = hTrkQual_OldVsNew->GetEntries();
  auto integral_old_trkqual = hTrkQual_OldVsNew->Integral(old_trkqual_min_bin, 100, 1, 100);
  auto integral_new_trkqual = hTrkQual_OldVsNew->Integral(1, 100, new_trkqual_min_bin, 100);
  std::cout << "passes old trkqual (all tracks) " << integral_old_trkqual << " / " << n_total_tracks << " = " << integral_old_trkqual / n_total_tracks << std::endl;
  std::cout << "passes new trkqual (all tracks) " << integral_new_trkqual << " / " << n_total_tracks << " = " << integral_new_trkqual / n_total_tracks << std::endl;

  auto integral_fails_both = hTrkQual_OldVsNew->Integral(1, old_trkqual_min_bin, 1, new_trkqual_min_bin);
  std::cout << "fails both cuts (all tracks) = " << integral_fails_both << std::endl;
  auto integral_passes_old_fails_new = hTrkQual_OldVsNew->Integral(old_trkqual_min_bin, 100, 1, new_trkqual_min_bin);
  std::cout << "passes old, fails new (all tracks) = " << integral_passes_old_fails_new << std::endl;
  auto integral_fails_old_passes_new = hTrkQual_OldVsNew->Integral(1, old_trkqual_min_bin, new_trkqual_min_bin, 100);
  std::cout << "fails old, passes new (all tracks) = " << integral_fails_old_passes_new << std::endl;
  auto integral_passes_both = hTrkQual_OldVsNew->Integral(old_trkqual_min_bin, 100, new_trkqual_min_bin, 100);
  std::cout << "passes both cuts (all tracks) = " << integral_passes_both << std::endl;

  TLatex* latex = new TLatex();
  latex->SetTextSize(0.06);
  latex->SetTextAlign(22);
  latex->SetTextColor(kRed);
  latex->DrawLatex(0.4, 0.4, Form("#splitline{%.0f}{fail both}", integral_fails_both));
  latex->DrawLatex(0.4, 1.0, Form("%.0f fail old, pass new", integral_fails_old_passes_new));
  latex->DrawLatex(0.95, 0.4, Form("%.0f", integral_passes_old_fails_new));
  latex->DrawLatex(1.0, 1.0, Form("%.0f", integral_passes_both));

  TCanvas* c2 = new TCanvas();
  c2->SetLogz();
  hTrkQual_OldVsNew_HQ->SetStats(false);
  hTrkQual_OldVsNew_HQ->SetTitle("True High-Quality Tracks");
  hTrkQual_OldVsNew_HQ->SetXTitle("TrkQual ANN1 v1");
  hTrkQual_OldVsNew_HQ->SetYTitle("TrkQual ANN1 v2");
  hTrkQual_OldVsNew_HQ->Draw("COLZ");
  old_trkqual_cut->Draw("LSAME");
  new_trkqual_cut->Draw("LSAME");

  integral_fails_both = hTrkQual_OldVsNew_HQ->Integral(1, old_trkqual_min_bin, 1, new_trkqual_min_bin);
  std::cout << "fails both cuts (HQ tracks) = " << integral_fails_both << std::endl;
  integral_passes_old_fails_new = hTrkQual_OldVsNew_HQ->Integral(old_trkqual_min_bin, 100, 1, new_trkqual_min_bin);
  std::cout << "passes old, fails new (HQ tracks) = " << integral_passes_old_fails_new << std::endl;
  integral_fails_old_passes_new = hTrkQual_OldVsNew_HQ->Integral(1, old_trkqual_min_bin, new_trkqual_min_bin, 100);
  std::cout << "fails old, passes new (HQ tracks) = " << integral_fails_old_passes_new << std::endl;
  integral_passes_both = hTrkQual_OldVsNew_HQ->Integral(old_trkqual_min_bin, 100, new_trkqual_min_bin, 100);
  std::cout << "passes both cuts (HQ tracks) = " << integral_passes_both << std::endl;

  latex->DrawLatex(0.4, 0.4, Form("#splitline{%.0f}{fail both}", integral_fails_both));
  latex->DrawLatex(0.4, 1.0, Form("%.0f fail old, pass new", integral_fails_old_passes_new));
  latex->DrawLatex(0.95, 0.4, Form("%.0f", integral_passes_old_fails_new));
  latex->DrawLatex(1.0, 1.0, Form("%.0f", integral_passes_both));

  TCanvas* c3 = new TCanvas();
  c3->SetLogz();
  hTrkQual_OldVsNew_LQ->SetStats(false);
  hTrkQual_OldVsNew_LQ->SetTitle("True Low-Quality Tracks");
  hTrkQual_OldVsNew_LQ->SetXTitle("TrkQual ANN1 v1");
  hTrkQual_OldVsNew_LQ->SetYTitle("TrkQual ANN1 v2");
  hTrkQual_OldVsNew_LQ->Draw("COLZ");
  old_trkqual_cut->Draw("LSAME");
  new_trkqual_cut->Draw("LSAME");


  integral_fails_both = hTrkQual_OldVsNew_LQ->Integral(1, old_trkqual_min_bin, 1, new_trkqual_min_bin);
  std::cout << "fails both cuts (LQ tracks) = " << integral_fails_both << std::endl;
  integral_passes_old_fails_new = hTrkQual_OldVsNew_LQ->Integral(old_trkqual_min_bin, 100, 1, new_trkqual_min_bin);
  std::cout << "passes old, fails new (LQ tracks) = " << integral_passes_old_fails_new << std::endl;
  integral_fails_old_passes_new = hTrkQual_OldVsNew_LQ->Integral(1, old_trkqual_min_bin, new_trkqual_min_bin, 100);
  std::cout << "fails old, passes new (LQ tracks) = " << integral_fails_old_passes_new << std::endl;
  integral_passes_both = hTrkQual_OldVsNew_LQ->Integral(old_trkqual_min_bin, 100, new_trkqual_min_bin, 100);
  std::cout << "passes both cuts (LQ tracks) = " << integral_passes_both << std::endl;

  latex->DrawLatex(0.4, 0.4, Form("#splitline{%.0f}{fail both}", integral_fails_both));
  latex->DrawLatex(0.4, 1.0, Form("%.0f fail old, pass new", integral_fails_old_passes_new));
  latex->DrawLatex(0.95, 0.4, Form("%.0f", integral_passes_old_fails_new));
  latex->DrawLatex(1.0, 1.0, Form("%.0f", integral_passes_both));

  if (save_plots) {
    std::string pngname = plotsdir + "/Mu2eTrkQual_CompareTrkQualTrainings_All.png";
    c1->SaveAs(pngname.c_str());

    pngname = plotsdir + "/Mu2eTrkQual_CompareTrkQualTrainings_HighQual.png";
    c2->SaveAs(pngname.c_str());

    pngname = plotsdir + "/Mu2eTrkQual_CompareTrkQualTrainings_LowQual.png";
    c3->SaveAs(pngname.c_str());
  }
}
