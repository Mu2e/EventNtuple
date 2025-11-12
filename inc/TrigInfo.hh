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

    bool _triggerArray[ntrig_];
    
    void reset() { *this = TrigInfo(); }
  };
}
#endif
