#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include <iostream>

using namespace rooutil;

void EventCounting(std::string filename) {

  RooUtil util(filename);
  std::cout << filename << " contains " << util.GetNEvents() << " events from " << util.GetNProcEvents() << " processed events." << std::endl;

}
