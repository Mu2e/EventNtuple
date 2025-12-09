//
// An example of how to create a flatter tree if you want to do an unbinned lieklihood fit
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

using namespace rooutil;
void CreateTrackNtuple(std::string filename) {

  // Create the TTree
  TTree* tree = new TTree("example_tree", "An example tree");

  // Here we will write out one track per row along with its track segment at the tracker entrance
  mu2e::TrkInfo trk;
  mu2e::TrkSegInfo trk_ent;
  tree->Branch("trk", &trk);
  tree->Branch("trk_ent", &trk_ent);

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
      trk = *(track.trk); // we will fill this data into the tree

      // Get the track segments at the tracker entrance
      auto trk_ent_segments = track.GetSegments([](TrackSegment& segment){ return tracker_entrance(segment) && has_reco_step(segment); });

      // Loop through the tracker entrance track segments
      for (const auto& segment : trk_ent_segments) {
        trk_ent = *(segment.trkseg); // we will fill this data into the tree
      }

      // Fill the tree once per track
      tree->Fill();
    }
  }

  // Create an output file
  TFile* outfile = new TFile("example_track_ntuple.root", "RECREATE");
  tree->Write();
  outfile->Close();
}
