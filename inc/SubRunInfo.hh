//
// SubRunInfo: subrun information
// Andy Edmonds (2026)
//
#ifndef SubRunInfo_HH
#define SubRunInfo_HH
#include <string>
namespace mu2e
{
  struct SubRunInfo {
    int run = 0; // run number
    int subrun = 0; // subrun number
    void reset() {*this = SubRunInfo(); }
  };
}
#endif
