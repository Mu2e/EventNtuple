//
// CaloDigiMCInfo: MC-truth information for calorimeter raw digis
//
#ifndef CaloDigiMCInfo_HH
#define CaloDigiMCInfo_HH

#include <vector>
#include "Offline/MCDataProducts/inc/MCRelationship.hh"

namespace mu2e
{
  struct CaloDigiMCInfo {

    int nsim;                           // # of sim particles associated with this digi
    float eDep;                         // total (corrected) deposited energy in the digi
    float eDepG4;                       // total G4 deposited energy in the digi
    float eprimary;                     // energy of the most energetic deposit
    float tprimary;                     // time of the most energetic deposit
    std::vector<float> eDeps;           // list of deposited energies
    std::vector<float> tDeps;           // list of times of energy deposits
    std::vector<float> momentumIns;     // list of the momentum of the SimParticle when entering in the disk
    std::vector<int> simParticleIds;    // list of simparticle ids
    std::vector<MCRelationship> simRels;// relationship to the particle that deposited the most energy in the calo digi
    int caloHitIdx_;                    // CaloHit index
    int crystalID_;                     // Crystal ID from CaloShowerSim
    int diskID_;           // Offline disk ID number
    float energyCorr_;                  // corrected energy from CaloShowerSim
    float timeCorr_;                    // corrected time from CaloShowerSim
    float posX_;                        // Crystal x position in detector frame
    float posY_;                        // Crystal y position in detector frame

    CaloDigiMCInfo() : nsim(0), eDep(0.0), eDepG4(0.0), eprimary(0.0), tprimary(0.0), caloHitIdx_(-1), crystalID_(-1), energyCorr_(0.0), timeCorr_(0.0), posX_(0.0), posY_(0.0) {}

    void reset() { *this = CaloDigiMCInfo(); }
  };
}
#endif
