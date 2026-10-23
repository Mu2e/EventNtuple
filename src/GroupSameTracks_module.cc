//
// Module that groups the fits in a KalSeedPtrCollection that were made from the same
// set of hits (e.g. the De, Ue, Dmu and Umu fits of one track), regardless of fit direction
// or particle hypothesis, and ranks the hypotheses within each group.
// The output KalSeedGroupCollection is parallel to the input KalSeedPtrCollection.
//
// Two fits are linked if the fraction of shared active hits (relative to the fit with fewer
// active hits) exceeds MinHitFrac; groups are the connected sets of linked fits.
//

#include "Offline/RecoDataProducts/inc/KalSeed.hh"

// Framework includes.
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "fhiclcpp/types/Atom.h"
#include "cetlib_except/exception.h"

#include "EventNtuple/inc/KalSeedGroup.hh"

// C++ includes.
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

namespace mu2e {

  class GroupSameTracks : public art::EDProducer {

  public:
    enum RankMethod { fitConsistency };

    struct Config {
      using Name=fhicl::Name;
      using Comment=fhicl::Comment;

      fhicl::Atom<int> debug{Name("debugLevel"), Comment("Debug level"), 0};
      fhicl::Atom<art::InputTag> kalSeedPtrTag{Name("KalSeedPtrCollection"), Comment("art::InputTag for the KalSeedPtrCollection to group")};
      fhicl::Atom<double> minHitFrac{Name("MinHitFrac"), Comment("Minimum fraction of shared active hits (relative to the fit with fewer active hits) for two fits to be the same track")};
      fhicl::Atom<std::string> rankMethod{Name("RankMethod"), Comment("Score used to rank the hypotheses in a group: FitConsistency")};
    };
    typedef art::EDProducer::Table<Config> Parameters;

    explicit GroupSameTracks(const Parameters& conf);

    void produce(art::Event& e) override;

  private:
    float score(KalSeed const& kseed) const;
    size_t findRoot(std::vector<size_t>& parent, size_t i) const;

    // fhicl parameters
    int _debug;
    art::InputTag _kalSeedPtrTag;
    double _minHitFrac;
    RankMethod _rankMethod;
  };

  GroupSameTracks::GroupSameTracks(const Parameters& conf):
    art::EDProducer(conf),
    _debug(conf().debug()),
    _kalSeedPtrTag(conf().kalSeedPtrTag()),
    _minHitFrac(conf().minHitFrac())
  {
    if (conf().rankMethod() == "FitConsistency") {
      _rankMethod = fitConsistency;
    } else {
      throw cet::exception("EventNtuple") << "GroupSameTracks: unknown RankMethod \"" << conf().rankMethod() << "\"";
    }
    consumes<KalSeedPtrCollection>(_kalSeedPtrTag);
    produces<KalSeedGroupCollection>();
  }

  float GroupSameTracks::score(KalSeed const& kseed) const {
    float value = 0;
    switch (_rankMethod) {
    case fitConsistency:
      value = kseed.fitConsistency();
      break;
    }
    // failed fits can report negative or NaN values; treat these as the worst possible score
    if (!std::isfinite(value) || value < 0) value = 0;
    return value;
  }

  size_t GroupSameTracks::findRoot(std::vector<size_t>& parent, size_t i) const {
    while (parent[i] != i) {
      parent[i] = parent[parent[i]]; // path halving
      i = parent[i];
    }
    return i;
  }

  void GroupSameTracks::produce(art::Event& event) {

    const auto& kspch = event.getValidHandle<KalSeedPtrCollection>(_kalSeedPtrTag);
    const auto& kspc = *kspch;
    const size_t nfits = kspc.size();

    // active hit indices of each fit
    std::vector<std::set<unsigned>> activeHits(nfits);
    for (size_t i_fit = 0; i_fit < nfits; ++i_fit) {
      for (const auto& hit : kspc[i_fit]->hits()) {
        if (hit._ambig > WireHitState::inactive) activeHits[i_fit].insert(hit._index);
      }
    }

    // link fits that share enough active hits
    std::vector<size_t> parent(nfits);
    std::iota(parent.begin(), parent.end(), 0);
    for (size_t i_fit = 0; i_fit < nfits; ++i_fit) {
      for (size_t j_fit = i_fit+1; j_fit < nfits; ++j_fit) {
        const auto& ihits = activeHits[i_fit];
        const auto& jhits = activeHits[j_fit];
        const size_t nmin = std::min(ihits.size(), jhits.size());
        if (nmin == 0) continue;
        size_t nshared = 0;
        for (auto index : ihits) {
          if (jhits.count(index) > 0) ++nshared;
        }
        const double hitFrac = double(nshared)/double(nmin);
        if (_debug > 1) std::cout << "GroupSameTracks: fits " << i_fit << " and " << j_fit << " share " << nshared << " of " << nmin << " active hits" << std::endl;
        if (hitFrac > _minHitFrac) {
          parent[findRoot(parent, j_fit)] = findRoot(parent, i_fit);
        }
      }
    }

    // number the groups in order of their first fit, and score each fit
    auto groups = std::make_unique<KalSeedGroupCollection>(nfits);
    std::vector<int> rootId(nfits, -1);
    int nGroups = 0;
    for (size_t i_fit = 0; i_fit < nfits; ++i_fit) {
      const size_t root = findRoot(parent, i_fit);
      if (rootId[root] < 0) rootId[root] = nGroups++;
      auto& group = groups->at(i_fit);
      group.kseed = kspc[i_fit];
      group.id = rootId[root];
      group.score = score(*kspc[i_fit]);
    }

    // rank = score normalised within the group; share equally if no fit in the group has a positive score
    std::vector<float> sumScore(nGroups, 0);
    std::vector<int> groupSize(nGroups, 0);
    for (const auto& group : *groups) {
      sumScore[group.id] += group.score;
      ++groupSize[group.id];
    }
    for (auto& group : *groups) {
      group.rank = sumScore[group.id] > 0 ? group.score/sumScore[group.id] : 1.0/groupSize[group.id];
      if (_debug > 0) std::cout << "GroupSameTracks: fit with PDG " << group.kseed->particle() << " in group " << group.id << " has score " << group.score << " and rank " << group.rank << std::endl;
    }

    event.put(std::move(groups));
  }
}

DEFINE_ART_MODULE(mu2e::GroupSameTracks)
