#ifndef MCParticle_hh_
#define MCParticle_hh_

#include <functional>
#include "EventNtuple/inc/SimInfo.hh"
#include "EventNtuple/inc/SurfaceStepInfo.hh"

struct MCParticle {
  MCParticle(mu2e::SimInfo* mcsim) : mcsim(mcsim) { }

  // Pointers to the data
  mu2e::SimInfo* mcsim = nullptr;
};

typedef std::function<bool(const MCParticle&)> MCParticleCut;
typedef std::vector<MCParticle> MCParticles;

#endif
