#include "EventNtuple/rooutil/inc/RooUtil.hh"

#include <iostream>

using namespace rooutil;

bool has_more_than_100_processed_events(const SubRun& subrun) {
  return subrun.has_proc_event_count && subrun.procEventCount > 100;
}

void SelectSubRuns(std::string filename) {
  RooUtil util(filename);

  const auto selected_subruns = util.GetSubRunIndices(has_more_than_100_processed_events);
  std::cout << "SubRuns with more than 100 processed events:" << std::endl;
  for (const auto i_subrun : selected_subruns) {
    const auto& subrun = util.GetSubRun(i_subrun);
    std::cout << subrun.srinfo->run << ":" << subrun.srinfo->subrun
              << " has " << subrun.procEventCount << " processed events" << std::endl;
  }

  if (util.GetNEvents() == 0) return;

  const auto& event = util.GetEvent(0);
  const auto* event_subrun = util.FindSubRun(event.evtinfo->run, event.evtinfo->subrun);
  if (event_subrun == nullptr) {
    std::cout << "No SubRun information is available for the first event." << std::endl;
    return;
  }

  std::cout << "First event belongs to " << event_subrun->srinfo->run << ":"
            << event_subrun->srinfo->subrun;
  if (event_subrun->has_cosmic_livetime) {
    std::cout << " with " << event_subrun->cosmicLivetime << " s of cosmic livetime";
  }
  std::cout << std::endl;
}
