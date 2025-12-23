#ifndef TrkCount_HH
#define TrkCount_HH
#include <string>
// TrkCount: counts track types and track-related quantities in an event (MARKED FOR REMOVAL)
// Dave Brown, LBNL 7/8/2016
// Modified by Sam Grant, ANL 12/22/2025
namespace mu2e
{
  struct TrkCount {
    int ntrk = 0; // number of distinct physical tracks collection

    void reset() { *this = TrkCount(); }
  };
}
#endif
