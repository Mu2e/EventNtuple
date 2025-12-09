//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

using namespace rooutil;
void PlotEntranceMomentumTriggeredEvents(std::string filename) {

  // Create the histogram you want to fill
  TH1F* hRecoMomAll = new TH1F("hRecoMomAll", "Reconstructed Momentum at Tracker Entrance", 50,95,110);
  TH1F* hRecoMom_tprDe_highP_stopTarg = new TH1F("hRecoMom_tprDe_highP_stopTarg", "Reconstructed Momentum at Tracker Entrance", 50,95,110);
  TH1F* hRecoMom_tprDe_highP = new TH1F("hRecoMom_tprDe_highP", "Reconstructed Momentum at Tracker Entrance", 50,95,110);

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
      auto trk_ent_segments = track.GetSegments([](TrackSegment& segment){ return tracker_entrance(segment) && has_reco_step(segment); });

      // Loop through the tracker entrance track segments
      for (auto& segment : trk_ent_segments) {

        // Fill the histogram
        hRecoMomAll->Fill(segment.trkseg->mom.R());

        if (passes_trigger(event, "tprDe_highP_stopTarg")) {
          hRecoMom_tprDe_highP_stopTarg->Fill(segment.trkseg->mom.R());
        }
        if (passes_trigger(event, "tprDe_highP_stopTarg")) {
          hRecoMom_tprDe_highP->Fill(segment.trkseg->mom.R());
        }
      }
    }
  }

  // Draw the histogram
  hRecoMomAll->SetLineColor(kBlack);
  hRecoMomAll->SetLineWidth(2);
  hRecoMomAll->Draw("HIST E");

  hRecoMom_tprDe_highP->SetLineColor(kBlue);
  hRecoMom_tprDe_highP->SetLineWidth(2);
  hRecoMom_tprDe_highP->Draw("HIST E SAME");

  hRecoMom_tprDe_highP_stopTarg->SetLineColor(kRed);
  hRecoMom_tprDe_highP_stopTarg->SetLineWidth(2);
  hRecoMom_tprDe_highP_stopTarg->Draw("HIST E SAME");

}
