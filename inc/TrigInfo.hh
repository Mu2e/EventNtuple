/*
 TrigInfo: stores information regarding the mu2e triggers passed or failed in the given event
 Sophie Middleton", 2025
*/
#ifndef TrigInfo_HH
#define TrigInfo_HH

namespace mu2e
{
  struct TrigInfo {

    static const unsigned int ntrig_ = 30; 

    std::vector<bool> triggerResults; 

    TrigInfo() : triggerResults(ntrig_, false) {}
    
    void reset() { *this = TrigInfo(); }
  };
}

    
// array of s of max length n_trig
// pass in strings from 
// fill the branch later on in a loop
#endif
