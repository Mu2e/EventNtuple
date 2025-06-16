#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

#include <iostream>

bool good_track(Track& track) {
  if (track.trk->fitcon > 1e-3 && is_e_minus(track)) {
    return true;
  }
  else {
    return false;
  }
}

void PrintEventsNoMC(std::string filename) {

  RooUtil util(filename);
  std::cout << filename << " has " << util.GetNEvents() << " events" << std::endl;

  // Now loop through the events and print the number of tracks in each event
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    auto& event = util.GetEvent(i_event);
    std::cout << "" << event.evtinfo->run << ":" << event.evtinfo->subrun << ":" << event.evtinfo->event << " has: " << std::endl;
    std::cout << "pbtime (reco) = " << event.evtinfo->pbtime << " ns" << std::endl;
    auto all_tracks = event.GetTracks();
    auto good_tracks = event.GetTracks(good_track);
    std::cout << all_tracks.size() << " total tracks and " << good_tracks.size() << " good tracks" << std::endl;

    int i_track = 0;
    for (auto& track : good_tracks) {
      ++i_track;
      std::cout << "  Track #" << i_track << " has " << track.trk->nhits << " hits and " << track.trk->nactive << " active hits (fit consistency = " << track.trk->fitcon << ")";

      auto all_segments = track.GetSegments([](TrackSegment& segment){ return has_reco_step(segment); });
      std::cout << "  and " << all_segments.size() << " segments:" << std::endl;
      for (auto& segment : all_segments) {
        std::cout << "    surfaceID " << segment.trkseg->sid << ": z = " << segment.trkseg->pos.z() << " mm, t = " << segment.trkseg->time << " ns, p = " << segment.trkseg->mom.R() << " MeV/c" << std::endl;
      }
    }
  }
}
