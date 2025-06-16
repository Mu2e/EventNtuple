#include <chrono>
#include <numeric>
#include <fstream>

using namespace std::chrono;

#include "EventNtuple/utils/rooutil/inc/RooUtil.hh"
#include "EventNtuple/utils/rooutil/inc/common_cuts.hh"

void TimingTest(std::string filename, bool log_output = false) {

  auto start_time = system_clock::now();

  // Set up RooUtil
  RooUtil util(filename);
  //  util.TurnOffAllBranches();
  //  std::vector<std::string> read_branches{"evtinfo", "evtinfomc", "trk", "trksegs"};
  //  util.TurnOnBranches(read_branches);

  auto const setup_time =
    duration_cast<nanoseconds>(system_clock::now() - start_time);

  vector<nanoseconds> times;
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    auto const t0 = system_clock::now();

    auto& event = util.GetEvent(i_event);

    times.push_back(duration_cast<nanoseconds>(system_clock::now() - t0));
  }

  auto const elapsed_time =
    duration_cast<nanoseconds>(system_clock::now() - start_time);
  auto const sum_times = std::accumulate(begin(times), end(times), nanoseconds(0));

  if (log_output) {
    std::fstream out("test_rooutil_timing.log", std::ios::app);
    out << times.size() << "," << sum_times.count() / times.size() << "," << elapsed_time.count() - setup_time.count() << "," << setup_time.count() << "," << elapsed_time.count() << "\n";
  }
  else {
    cout << "Processed " << times.size() << " events in an average of "
         << (sum_times.count() / times.size())*1e-3 << " microseconds/event = " << 1/((sum_times.count() / times.size())*1e-9) << " events per second\n";
    cout << "Total EventLoop time was "
         << (elapsed_time.count() - setup_time.count())*1e-6 << " milliseconds\n";
    cout << "Setup time = " << setup_time.count()*1e-6 << " milliseconds\n";
    cout << "Total Time was " << elapsed_time.count()*1e-6 << " milliseconds\n";
  }
}
