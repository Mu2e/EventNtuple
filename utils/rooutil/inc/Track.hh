#ifndef Track_hh_
#define Track_hh_

#include "TrkAna/inc/TrkInfo.hh"
#include "TrkAna/inc/TrkFitInfo.hh"

#include "TrkAna/utils/rooutil/inc/TrackSegment.hh"

struct Track {
  Track(mu2e::TrkInfo* trk, std::vector<mu2e::TrkFitInfo>* trkfit, bool debug = false) : trk(trk), trkfit(trkfit), debug(debug) {

    // Create the underlying track segments
    for (int i_segment = 0; i_segment < nSegments(); ++i_segment) {
      TrackSegment segment(&(trkfit->at(i_segment))); // passing the addresses of the underlying structs
      if (debug) {
        std::cout << "Track::Track(): Adding segment #" << i_segment << " to segments..." << std::endl;
      }
      segments.emplace_back(segment);
    }

    // Time order the segments
    std::sort(segments.begin(), segments.end(), TrackSegment::earliest);

    if (debug) {
      std::cout << "Track::Track(): All done." << std::endl;
    }
  }

  int nSegments() const { return trkfit->size(); }

  TrackSegments GetSegments() const { return segments; }
  TrackSegments GetSegments(TrackSegmentCut cut) const {
    TrackSegments select_segments;
    for (const auto& segment : segments) {
      if (cut(segment)) {
        select_segments.emplace_back(segment);
      }
    }
    return select_segments;
  }

  TrackSegments segments;
  bool debug = false;

  // Pointers to the data
  mu2e::TrkInfo* trk = nullptr;
  std::vector<mu2e::TrkFitInfo>* trkfit = nullptr;
};

typedef bool (*TrackCut)(const Track& track);
typedef std::vector<Track> Tracks;

#endif