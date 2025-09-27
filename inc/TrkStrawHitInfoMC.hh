//
// TrkStrawHitInfoMC: MC-truth information of straw hits assigned to a track
//
#ifndef TrkStrawHitInfoMC_HH
#define TrkStrawHitInfoMC_HH
#include "EventNtuple/inc/RootVectors.hh"
#include "Offline/MCDataProducts/inc/MCRelationship.hh"
namespace mu2e
{
   struct TrkStrawHitInfoMC {
    int pdg = -1;
    int gen = -1;
    int startCode = -1; // PDG particle code, generator code and process code of the particle which caused the electronics to cross threshold in simulation
    int ambig = -100;   // true left-right ambiguity = true angular momentum sign of the particle WRT the wire
    int earlyend = -1;   // true end of the early signal (cal =0 or HV=1)
    MCRelationship rel = MCRelationship::none; // relationship (same, mother, daughter, sibling, unrelated) of this particle to the particle generating most of the hits on this track
    float t0 = -1000.0;  // true time this particle passed closest to this wire
    float tdrift = -1000.0;  // true drift time
    float rdrift = -1000.0;  // true drift time
    float tprop = -1000.0;  // true signal propagation time
    float dist = -1000.0;  // true transverse distance between the cluster and the wire
    float doca = -1000.0;  // true transverse distance at POCA of the particle to the wire
    float len = -1000.0;   // true distance from the cluster to the straw center
    float edep = -1000.0;  // true energy deposit sum by trigger particles in the straw gas
    float mom = -1000.0;   // true particle momentum at the POCA
    float twdot = -1000.0; // dot product between track and wire directions
    float tau = -1000.0; // threshold cluster distance to the wire along the perpedicular particle path
    float cdist = -1000.0; // threshold cluster distance to the wire
    float phi = -1000.0; // cylindrical phi around wire of POCA
    float lang = -1000.0; // lorentz angle
    float strawdoca = -1000.0; // true transverse distance at POCA of the particle to the straw center
    float strawphi = -1000.0; // cylindrical phi around straw center of POCA
    XYZVectorF cpos; // threshold cluster position
    bool recohit = true; // Is this MC info associated with a reco hit or not
  };
}
#endif
