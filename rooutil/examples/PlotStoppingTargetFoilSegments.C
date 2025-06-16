//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"
#include "TLegend.h"

void PlotStoppingTargetFoilSegments(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hZ_Reco = new TH1F("hZ_Reco", "Z TrkSeg",40,0,40);
  TH1F* hZ_MC = new TH1F("hZ_MC", "Z TrkSeg",40,0,40);

  // Set up RooUtil
  RooUtil util(filename);
  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto e_minus_tracks = event.GetTracks(is_e_minus);

    // Loop through the e_minus tracks
    for (auto& track : e_minus_tracks) {

      // Get the track segments at the tracker entrance
      auto st_foil_segments = track.GetSegments(stopping_target_foils);

      // Loop through the tracker entrance track segments
      for (auto& segment : st_foil_segments) {
        if (has_mc_step(segment)) {
          hZ_MC->Fill(segment.trksegmc->sindex);
        }
        if (has_reco_step(segment)) {
          hZ_Reco->Fill(segment.trkseg->sindex);
        }
      }
    }
  }

  // Draw the histogram
  hZ_Reco->SetLineColor(kBlack);
  hZ_Reco->SetLineWidth(2);
  hZ_Reco->SetXTitle("ST Foil Number");
  hZ_Reco->SetYTitle("N Entries");
  hZ_Reco->Draw("HIST E");
  hZ_MC->SetLineColor(kRed);
  hZ_MC->SetLineWidth(2);
  hZ_MC->Draw("HIST E SAME");

  TLegend* leg = new TLegend(0.10, 0.70, 0.30, 0.90);
  leg->AddEntry(hZ_Reco, "Reco", "l");
  leg->AddEntry(hZ_MC, "MC", "l");
  leg->Draw();
}
