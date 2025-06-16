//
// An example of how to plot the momentum of electrons at the tracker entrance
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

#include "TH1F.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TGaxis.h"

void PlotEntranceFitPars(std::string filename) {

  double xmin = -800;
  double xmax = 800;
  double ymin = -800;
  double ymax = 800;

  TCanvas* c = new TCanvas("c1", "c1", 800,800);
  double extra = 200;
  c->Range(xmin-extra, ymin-extra, xmax+extra, ymax+extra);
  TGaxis* x_axis = new TGaxis(xmin, ymin, xmax, ymin, xmin,xmax);
  x_axis->Draw();
  TGaxis* y_axis = new TGaxis(xmin, ymin, xmin, ymax, ymin,ymax);
  y_axis->Draw();

  TEllipse* tracker_rmin = new TEllipse(0, 0, 380, 380);
  tracker_rmin->SetFillStyle(0);
  tracker_rmin->Draw();
  TEllipse* tracker_rmax = new TEllipse(0, 0, 700,700);
  tracker_rmax->SetFillStyle(0);
  tracker_rmax->Draw("LSAME");

  // Set up RooUtil
  RooUtil util(filename);

  // Loop through the events
  for (int i_event = 0; i_event < 1; ++i_event) {
    // Get the next event
    auto& event = util.GetEvent(i_event);

    // Get the e_minus tracks from the event
    auto e_minus_tracks = event.GetTracks(is_e_minus);

    // Loop through the e_minus tracks
    for (auto& track : e_minus_tracks) {

      // Get the track segments at the tracker entrance
      auto trk_ent_segments = track.GetSegments(tracker_entrance);

      // Loop through the tracker entrance track segments
      for (auto& segment : trk_ent_segments) {

        TEllipse* helix = new TEllipse(segment.trksegpars_lh->cx, segment.trksegpars_lh->cy, segment.trksegpars_lh->rad, segment.trksegpars_lh->rad);
        helix->SetLineColor(kRed);
        helix->SetFillStyle(0);
        helix->Draw();
      }
    }
  }
}
