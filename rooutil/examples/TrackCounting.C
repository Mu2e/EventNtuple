#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include <iostream>

using namespace rooutil;
bool good_track(Track& track) {
  if (track.trk->fitcon > 1e-3) {
    return true;
  }
  else {
    return false;
  }
}

void TrackCounting(std::string filename) {

  RooUtil util(filename);
  std::cout << filename << " has " << util.GetNEvents() << " events" << std::endl;

  // Now loop through the events and print the number of tracks in each event
  std::cout << "Run\tSubrun\tEvent\tN_trk(good)\tN_e-(good)\tN_e+(good)\tN_mu-(good)\tN_mu+(good)" << std::endl;
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    auto& event = util.GetEvent(i_event);

    int n_total_tracks = event.CountTracks();
    int n_e_minus_tracks = event.CountTracks(is_e_minus);
    int n_e_plus_tracks = event.CountTracks(is_e_plus);
    int n_mu_minus_tracks = event.CountTracks(is_mu_minus);
    int n_mu_plus_tracks = event.CountTracks(is_mu_plus);

    int n_good_tracks = event.CountTracks(good_track);
    int n_e_minus_good_tracks = event.CountTracks([](Track& track){ return is_e_minus(track) && good_track(track); });
    int n_e_plus_good_tracks = event.CountTracks([](Track& track){ return is_e_plus(track) && good_track(track); });
    int n_mu_minus_good_tracks = event.CountTracks([](Track& track){ return is_mu_minus(track) && good_track(track); });
    int n_mu_plus_good_tracks = event.CountTracks([](Track& track){ return is_mu_plus(track) && good_track(track); });

    std::cout << event.evtinfo->run << "\t" << event.evtinfo->subrun << "\t" << event.evtinfo->event << "\t" << n_total_tracks << " (" << n_good_tracks << ")\t" << n_e_minus_tracks << " (" << n_e_minus_good_tracks << ")\t" << n_e_plus_tracks << " (" << n_e_plus_good_tracks << ")\t" << n_mu_minus_tracks << " (" << n_mu_minus_good_tracks << ")\t" << n_mu_plus_tracks << " (" << n_mu_plus_good_tracks << ")" << std::endl;

  }
}
