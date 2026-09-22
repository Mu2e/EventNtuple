#include "EventNtuple/rooutil/inc/RooUtil.hh"

#include <iostream>

using namespace rooutil;

void SubRunCounting(std::string filename) {
  RooUtil util(filename);

  std::cout << filename << " contains " << util.GetNSubRuns() << " subruns" << std::endl;
  for (int i_subrun = 0; i_subrun < util.GetNSubRuns(); ++i_subrun) {
    const auto& subrun = util.GetSubRun(i_subrun);
    std::cout << "run " << subrun.srinfo->run << ", subrun " << subrun.srinfo->subrun;
    if (subrun.has_gen_event_count) { std::cout << ": " << subrun.genEventCount << " generated events"; }
    if (subrun.has_proc_event_count) { std::cout << ", " << subrun.procEventCount << " processed events"; }
    if (subrun.has_cosmic_livetime) { std::cout << ", " << subrun.cosmicLivetime << " s of cosmic livetime"; }
    std::cout << std::endl;
  }

  if (const auto generated_events = util.GetGeneratedEvents()) {
    std::cout << "Total generated events: " << *generated_events << std::endl;
  }
  if (const auto processed_events = util.GetProcessedEvents()) {
    std::cout << "Total processed events: " << *processed_events << std::endl;
  }
  if (const auto cosmic_livetime = util.GetCosmicLivetime()) {
    std::cout << "Total cosmic livetime: " << *cosmic_livetime << " s" << std::endl;
  }
}
