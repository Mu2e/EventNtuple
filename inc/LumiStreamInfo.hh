//
// LumiStreamInfo: basic information available in the lumi stream
// Michael MacKenzie
//
#ifndef LumiStreamInfo_HH
#define LumiStreamInfo_HH

#include <vector>
namespace mu2e
{
  struct LumiStreamInfo {
    int nCaloHitsD0                       =  0; // N(calorimeter hits) in disk 0
    int nCaloHitsD1                       =  0; // N(calorimeter hits) in disk 1
    int caloEnergy                        =  0; // Total energy in the calorimeter (stored as an integer)
    std::vector<double> caphriHitEnergies = {}; // List of CAPHRI hit energies
    std::vector<int>    caphriHitIDs      = {}; // List of CAPHRI hit crystal IDs
    int nTrackerHits                      =  0; // N(tracker hits) in the event
    int nProtonTCs                        =  0; // N(proton time clusters) in the event
    void reset() { *this = LumiStreamInfo(); }
  };
}
#endif
