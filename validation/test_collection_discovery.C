//
// test_collection_discovery.C - self-contained test of RooUtil's automatic discovery of the time
// cluster and line seed collections, which a job may write under any number of names (Run1B writes
// e.g. "timeclusters", "tztimeclusters", "lineseeds" and "cosmiclineseeds").
//
// It writes a small synthetic EventNtuple file holding both conventionally-named and
// alternately-named collections, then checks that
//   * both are discovered by branch class, before any event has been read,
//   * *CollectionNames() enumerates them,
//   * Has*(name)/Get*(name) return the right collection, keyed by name, and
//   * in-place selection keeps exactly the entries that pass the cut, in the wrappers, in the
//     named collections and in the underlying branch, and
//   * the companion "<collection>hits" combo hit lists are picked up, attached to the right entry
//     of the right collection, and kept index-aligned through an in-place selection.
//
// Usage: root -l -b -q EventNtuple/validation/test_collection_discovery.C+
// Prints "test_collection_discovery: PASSED" and returns 0 on success, the number of failed
// checks otherwise.
//
#include "EventNtuple/rooutil/inc/Event.hh"

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>

namespace {
  int n_failures = 0;

  void Check(bool passed, const std::string& what) {
    std::cout << (passed ? "  PASS: " : "  FAIL: ") << what << std::endl;
    if (!passed) { ++n_failures; }
  }

  // t0 identifies the entry, so we can tell the collections (and the survivors of a cut) apart
  mu2e::EventNtupleTimeClusterInfo MakeTimeCluster(float t0, int nhits) {
    mu2e::EventNtupleTimeClusterInfo info;
    info.t0 = t0;
    info.nhits = nhits;
    info.nStrawHits = 2*nhits;
    return info;
  }

  mu2e::LineSeedInfo MakeLineSeed(float t0, int nhits) {
    mu2e::LineSeedInfo info;
    info.t0 = t0;
    info.nhits = nhits;
    info.nStrawHits = 2*nhits;
    return info;
  }

  // One combo hit per entry, tagged by time so we can tell which entry it was attached to
  std::vector<mu2e::EventNtupleComboHitInfo> MakeHits(float time, int n_hits) {
    std::vector<mu2e::EventNtupleComboHitInfo> hits;
    for (int i_hit = 0; i_hit < n_hits; ++i_hit) {
      mu2e::EventNtupleComboHitInfo hit;
      hit.time = time;
      hit.nStrawHits = i_hit + 1;
      hits.emplace_back(hit);
    }
    return hits;
  }

  // Write a minimal EventNtuple-like file with two time cluster and two line seed collections, one
  // of each under a name RooUtil does not know about ahead of time
  void WriteTestFile(const std::string& filename) {
    TFile file(filename.c_str(), "RECREATE");
    file.mkdir("EventNtuple")->cd();
    TTree* ntuple = new TTree("ntuple", "test ntuple");

    std::vector<mu2e::EventNtupleTimeClusterInfo> timeclusters, tztimeclusters;
    std::vector<mu2e::LineSeedInfo> lineseeds, cosmiclineseeds;
    // hit lists are written per collection, and only for the collections a job asks for: give them
    // to the conventional time clusters and to the alternately-named line seeds, so both the
    // dedicated-pointer path and the discovered-branch path are covered, along with two
    // collections that have none
    std::vector<std::vector<mu2e::EventNtupleComboHitInfo>> timeclustershits, cosmiclineseedshits;
    ntuple->Branch("timeclusters", &timeclusters);
    ntuple->Branch("timeclustershits", &timeclustershits);
    ntuple->Branch("tztimeclusters", &tztimeclusters);
    ntuple->Branch("lineseeds", &lineseeds);
    ntuple->Branch("cosmiclineseeds", &cosmiclineseeds);
    ntuple->Branch("cosmiclineseedshits", &cosmiclineseedshits);

    for (int i_event = 0; i_event < 2; ++i_event) {
      timeclusters    = { MakeTimeCluster(100.f, 10), MakeTimeCluster(200.f, 20) };
      tztimeclusters  = { MakeTimeCluster(300.f, 30) };
      lineseeds       = { MakeLineSeed(400.f, 40), MakeLineSeed(500.f, 50) };
      cosmiclineseeds = { MakeLineSeed(600.f, 60) };
      timeclustershits    = { MakeHits(100.f, 3), MakeHits(200.f, 4) };
      cosmiclineseedshits = { MakeHits(600.f, 5) };
      ntuple->Fill();
    }
    ntuple->Write();
    file.Close();
  }
} // anonymous namespace

int test_collection_discovery(std::string filename = "nts.test.collection_discovery.root") {
  WriteTestFile(filename);

  TChain ntuple("EventNtuple/ntuple");
  ntuple.Add(filename.c_str());
  rooutil::Event event(&ntuple); // discovery happens here, in the constructor

  // -- the collections are discovered when the file is opened, so their names are available before
  // -- any event has been read (e.g. to decide what to book histograms for)
  std::cout << "Collection discovery (before reading any event):" << std::endl;
  const auto tc_names = event.TimeClusterCollectionNames();
  const auto ls_names = event.LineSeedCollectionNames();
  Check(tc_names.size() == 2, "two time cluster collections discovered (found " + std::to_string(tc_names.size()) + ")");
  Check(ls_names.size() == 2, "two line seed collections discovered (found " + std::to_string(ls_names.size()) + ")");
  Check(event.HasTimeClusters("timeclusters"), "HasTimeClusters(\"timeclusters\")");
  Check(event.HasTimeClusters("tztimeclusters"), "HasTimeClusters(\"tztimeclusters\")");
  Check(!event.HasTimeClusters("notacollection"), "!HasTimeClusters(\"notacollection\")");
  Check(!event.HasTimeClusters("lineseeds"), "time cluster and line seed collections are not mixed up");
  Check(event.HasLineSeeds("lineseeds"), "HasLineSeeds(\"lineseeds\")");
  Check(event.HasLineSeeds("cosmiclineseeds"), "HasLineSeeds(\"cosmiclineseeds\")");
  Check(!event.HasLineSeeds("notacollection"), "!HasLineSeeds(\"notacollection\")");
  Check(event.HasTimeClusterHits("timeclusters"), "HasTimeClusterHits(\"timeclusters\")");
  Check(!event.HasTimeClusterHits("tztimeclusters"), "!HasTimeClusterHits(\"tztimeclusters\") -- no hit branch written");
  Check(event.HasLineSeedHits("cosmiclineseeds"), "HasLineSeedHits(\"cosmiclineseeds\")");
  Check(!event.HasLineSeedHits("lineseeds"), "!HasLineSeedHits(\"lineseeds\") -- no hit branch written");

  // -- named access returns the right collection once an event has been read
  std::cout << "Named access (after reading an event):" << std::endl;
  ntuple.GetEntry(0);
  event.Update();
  Check(event.GetTimeClusters("timeclusters").size() == 2, "GetTimeClusters(\"timeclusters\") has 2 entries");
  Check(event.GetTimeClusters("tztimeclusters").size() == 1, "GetTimeClusters(\"tztimeclusters\") has 1 entry");
  Check(event.GetTimeClusters("tztimeclusters").at(0).T0() == 300.f, "GetTimeClusters(\"tztimeclusters\") holds that branch's data");
  Check(event.GetTimeClusters("notacollection").empty(), "GetTimeClusters() of an unknown name is empty");
  Check(event.GetLineSeeds("lineseeds").size() == 2, "GetLineSeeds(\"lineseeds\") has 2 entries");
  Check(event.GetLineSeeds("cosmiclineseeds").size() == 1, "GetLineSeeds(\"cosmiclineseeds\") has 1 entry");
  Check(event.GetLineSeeds("cosmiclineseeds").at(0).lineseed->t0 == 600.f, "GetLineSeeds(\"cosmiclineseeds\") holds that branch's data");
  Check(event.GetLineSeeds("notacollection").empty(), "GetLineSeeds() of an unknown name is empty");

  // -- each entry carries its own collection's hit list, and only where one was written
  std::cout << "Combo hit lists:" << std::endl;
  const auto& tcs = event.GetTimeClusters("timeclusters");
  Check(tcs.size() == 2 && tcs.at(0).HasHits() && tcs.at(1).HasHits(), "both timeclusters entries have hits");
  if (tcs.size() == 2 && tcs.at(0).HasHits() && tcs.at(1).HasHits()) {
    Check(tcs.at(0).NComboHits() == 3 && tcs.at(1).NComboHits() == 4, "each time cluster gets its own hit list");
    Check(tcs.at(0).Hits().at(0).time == 100.f && tcs.at(1).Hits().at(0).time == 200.f,
          "the hit lists are attached to the matching time cluster");
  }
  Check(!event.GetTimeClusters("tztimeclusters").at(0).HasHits(), "a collection with no hit branch has no hits");
  Check(event.GetTimeClusters("tztimeclusters").at(0).Hits().empty(), "Hits() of an entry with no hit list is empty");

  const auto& cosmics = event.GetLineSeeds("cosmiclineseeds");
  Check(cosmics.size() == 1 && cosmics.at(0).HasHits(), "the cosmiclineseeds entry has hits");
  if (cosmics.size() == 1 && cosmics.at(0).HasHits()) {
    Check(cosmics.at(0).NComboHits() == 5, "the line seed hit list has the right size");
    Check(cosmics.at(0).Hits().at(0).time == 600.f, "the hit list is attached to the matching line seed");
  }
  Check(!event.GetLineSeeds("lineseeds").at(0).HasHits(), "a line seed collection with no hit branch has no hits");

  // -- the conventional branches stay reachable through the dedicated accessors as well
  Check(event.GetTimeClusters().size() == 2, "GetTimeClusters() still returns the \"timeclusters\" branch");
  Check(event.GetLineSeeds().size() == 2, "GetLineSeeds() still returns the \"lineseeds\" branch");

  // -- in-place selection keeps the entries passing the cut. The cut here rejects the *first* entry
  // -- and keeps the second, which is the case a std::remove_if-based implementation gets backwards
  // -- (the tail it leaves behind holds the retained entry, not the rejected one).
  std::cout << "In-place selection:" << std::endl;
  event.GetLineSeeds([](rooutil::LineSeed& seed) { return seed.lineseed->t0 > 450.f; }, true);
  Check(event.GetLineSeeds().size() == 1, "one line seed survives the cut");
  Check(event.nLineSeeds() == 1, "the underlying lineseeds branch has one entry left");
  if (event.GetLineSeeds().size() == 1) {
    Check(event.GetLineSeeds().at(0).lineseed->t0 == 500.f, "the surviving line seed is the one that passed the cut");
  }
  if (event.nLineSeeds() == 1) { // check the branch itself, not just the wrapper pointing into it
    Check(event.lineseeds->at(0).t0 == 500.f, "the entry left in the lineseeds branch is the one that passed the cut");
  }
  Check(event.GetLineSeeds("lineseeds").size() == 1, "the named \"lineseeds\" collection tracks the selection");
  Check(event.GetLineSeeds("cosmiclineseeds").size() == 1, "an unselected line seed collection is untouched");

  event.GetTimeClusters([](rooutil::TimeCluster& cluster) { return cluster.T0() > 150.f; }, true);
  Check(event.GetTimeClusters().size() == 1, "one time cluster survives the cut");
  Check(event.nTimeClusters() == 1, "the underlying timeclusters branch has one entry left");
  if (event.GetTimeClusters().size() == 1) {
    Check(event.GetTimeClusters().at(0).T0() == 200.f, "the surviving time cluster is the one that passed the cut");
  }
  if (event.nTimeClusters() == 1) {
    Check(event.timeclusters->at(0).t0 == 200.f, "the entry left in the timeclusters branch is the one that passed the cut");
  }
  Check(event.GetTimeClusters("timeclusters").size() == 1, "the named \"timeclusters\" collection tracks the selection");
  Check(event.GetTimeClusters("tztimeclusters").size() == 1, "an unselected time cluster collection is untouched");
  if (event.GetTimeClusters().size() == 1) { // the hit lists must be erased alongside the clusters
    Check(event.GetTimeClusters().at(0).NComboHits() == 4, "the surviving time cluster kept its own hit list");
    Check(event.timeclustershits->size() == 1, "the timeclustershits branch has one entry left");
  }

  // -- reading the next event refreshes everything
  std::cout << "Re-reading:" << std::endl;
  ntuple.GetEntry(1);
  event.Update();
  Check(event.GetLineSeeds().size() == 2, "the next event restores the full line seed collection");
  Check(event.GetTimeClusters("timeclusters").size() == 2, "the next event restores the full time cluster collection");

  if (n_failures == 0) { std::cout << "test_collection_discovery: PASSED" << std::endl; }
  else                 { std::cout << "test_collection_discovery: FAILED (" << n_failures << " checks)" << std::endl; }
  return n_failures;
}
