//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"
#include "TCanvas.h"

void PlotEntranceMomentumResolution_TrkQualCut(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hRecoMomRes = new TH1F("hRecoMomRes", "Momentum Resolution at Tracker Entrance", 200,-10,10);
  TH1F* hRecoMomRes_TrkQualCut = new TH1F("hRecoMomRes_TrkQualCut", "Momentum Resolution at Tracker Entrance", 200,-10,10);

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

      // Get the track segments at the tracker entrance and has an MC step
      auto trk_ent_segments = track.GetSegments([](TrackSegment& segment){ return tracker_entrance(segment) && has_mc_step(segment) && has_reco_step(segment); });

      // Loop through the tracker entrance track segments
      for (auto& segment : trk_ent_segments) {

        hRecoMomRes->Fill(segment.trkseg->mom.R() - segment.trksegmc->mom.R());

        if (passes_trkqual(track, 0.2)) {
          // Fill the histogram
          hRecoMomRes_TrkQualCut->Fill(segment.trkseg->mom.R() - segment.trksegmc->mom.R());
        }
      }
    }
  }

  // Draw the histogram
  TCanvas* c1 = new TCanvas();
  c1->SetLogy();
  hRecoMomRes->SetLineColor(kBlue);
  hRecoMomRes->Draw("HIST E");
  hRecoMomRes_TrkQualCut->SetLineColor(kRed);
  hRecoMomRes_TrkQualCut->Draw("HIST E SAME");
}
