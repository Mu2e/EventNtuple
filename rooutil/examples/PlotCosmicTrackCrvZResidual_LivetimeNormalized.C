#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TCanvas.h"
#include "TH1F.h"

#include <iostream>

using namespace rooutil;

bool crv_top_surface(TrackSegment& segment) {
  if (segment.trkseg == nullptr) return false;

  const int surface_id = segment.trkseg->sid;
  return surface_id == mu2e::SurfaceIdDetail::TCRV; // note: this is a deprecated surface id, but it is what is in the file I'm testing
}

void PlotCosmicTrackCrvZResidual_LivetimeNormalized(std::string filename) {
  RooUtil util(filename);
  TH1F* crv_track_z_residual = new TH1F("crv_track_z_residual", "CRV coincidence-track z residual;z_{CRV coincidence} - z_{track} [mm];tracks / s", 100, -500, 500);

  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    auto& event = util.GetEvent(i_event);
    const auto tracks = event.GetTracks();
    auto crv_coincs = event.GetCrvCoincs();
    for (auto track : tracks) {
      auto crv_top_segments = track.GetSegments([](TrackSegment& segment) {
        return crv_top_surface(segment) && has_reco_step(segment);
      });
      for (auto& segment : crv_top_segments) {
        for (auto& crv_coinc : crv_coincs) {
          if (track_crv_coincidence(segment, crv_coinc)) {
            crv_track_z_residual->Fill(crv_coinc.reco->pos.z() - segment.trkseg->pos.z());
          }
        }
      }
    }
  }

  const auto entries = crv_track_z_residual->Integral();
  if (entries > 0.0) {
    if (const auto rate = util.GetRate(entries)) {
      crv_track_z_residual->Scale(*rate / entries);
    } else {
      std::cout << "Cannot normalize histogram: cosmic livetime is unavailable." << std::endl;
    }
  }

  TCanvas* canvas = new TCanvas("canvas", "CRV coincidence-track z residual");
  crv_track_z_residual->Draw("HIST");
  canvas->SaveAs("crv_track_z_residual_rate.pdf");
}
