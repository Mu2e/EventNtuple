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
    int nCaloHitsD0                       = -1;    // N(calorimeter hits) in disk 0
    int nCaloHitsD1                       = -1;    // N(calorimeter hits) in disk 1
    int caloEnergy                        = -1;    // Total energy in the calorimeter (stored as an integer) in MeV
    std::vector<double> caphriHitEnergies = {};    // List of CAPHRI hit energies in MeV
    std::vector<int>    caphriHitIDs      = {};    // List of CAPHRI hit crystal IDs
    bool                caphriIsValid     = false; // Flag for the CAPHRI list being valid
    int nTrackerHits                      = -1;    // N(tracker hits) in the event
    int nProtonTCs                        = -1;    // N(proton time clusters) in the event
    void reset() { *this = LumiStreamInfo(); }
  };
}
#endif
