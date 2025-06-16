//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

void PlotTrackHitTimesMC(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hTrackHitTimesMCUnused = new TH1F("hTrackHitTimesMCUnused", "Unused MC Track Hit Times", 170,0,1700);
  TH2F* hTrackHitTimesRecoVsMC = new TH2F("hTrackHitTimesRecoVsMC", "Reco vs MC Track Hit Times", 170,0,1700, 170,0,1700);
  hTrackHitTimesRecoVsMC->SetXTitle("MC");
  hTrackHitTimesRecoVsMC->SetYTitle("Reco");

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    // Get the next event
    auto event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto e_minus_tracks = event.GetTracks(is_e_minus);

    // Loop through the e_minus tracks
    for (auto& track : e_minus_tracks) {

      // Get the track segments at the tracker entrance
      auto trkhits = track.GetHits();

      // Loop through the tracker entrance track segments
      for (auto& trkhit : trkhits) {
        if (trkhit.reco != nullptr) {
          hTrackHitTimesRecoVsMC->Fill(trkhit.reco->ptoca, trkhit.mc->t0);
        }
        else {
          hTrackHitTimesMCUnused->Fill(trkhit.mc->t0);
        }
      }
    }
  }

  // Draw the histogram
  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->Divide(2,1);
  c1->cd(1);
  hTrackHitTimesMCUnused->Draw("HIST E");
  c1->cd(2);
  hTrackHitTimesRecoVsMC->Draw("COLZ");
}
