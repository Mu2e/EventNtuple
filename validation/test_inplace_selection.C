//
// Regression test for in-place GetTracks/GetCaloClusters selection (Mu2e/EventNtuple#416)
// Checks the backing branch vectors and the rebuilt wrappers hold the entries that pass the cut,
// including events where a rejected entry comes before a kept one.
//
// root -l -b -q validation/test_inplace_selection.C++\(\"nts.file.root\"\)
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"

#include <iostream>
#include <vector>

using namespace rooutil;

int test_inplace_selection(std::string filename, int max_events = 1000) {
  RooUtil util(filename);
  int n_fail = 0, n_checked = 0, n_reject_before_keep = 0, n_calo_checked = 0, n_calo_rbk = 0;
  const int min_nactive = 20;
  auto trk_cut = [&](Track& t) { return t.trk->nactive >= min_nactive; };
  auto calo_cut = [&](CaloCluster& c) { return c.calocluster->diskID_ == 1; }; // clusters are energy-ordered, so this rejects before keeping

  const int n_events = std::min<int>(util.GetNEvents(), max_events);
  for (int i_event = 0; i_event < n_events; ++i_event) {
    auto& event = util.GetEvent(i_event);

    // expected: identifying values of the tracks/clusters that pass, in order
    std::vector<int> exp_trk; std::vector<double> exp_calo;
    bool seen_reject = false, reject_before_keep = false;
    for (auto t : event.GetTracks()) {
      if (trk_cut(t)) { exp_trk.push_back(t.trk->nactive); if (seen_reject) reject_before_keep = true; }
      else { seen_reject = true; }
    }
    bool calo_seen_reject = false, calo_rbk = false;
    for (auto c : event.GetCaloClusters()) {
      if (calo_cut(c)) { exp_calo.push_back(c.calocluster->energyDep_); if (calo_seen_reject) calo_rbk = true; }
      else { calo_seen_reject = true; }
    }

    event.SelectTracks(trk_cut);
    event.GetCaloClusters(calo_cut, true);

    bool ok = true;
    // wrappers
    auto tracks = event.GetTracks();
    if (tracks.size() != exp_trk.size()) ok = false;
    else for (size_t i = 0; i < tracks.size(); ++i) if (tracks[i].trk->nactive != exp_trk[i]) ok = false;
    // backing branches (what would be written out)
    if (event.trk->size() != exp_trk.size()) ok = false;
    else for (size_t i = 0; i < exp_trk.size(); ++i) {
      if (event.trk->at(i).nactive != exp_trk[i]) ok = false;
      if (tracks[i].trk != &(event.trk->at(i))) ok = false; // wrapper points into the vector
    }
    if (event.trkhits && event.trkhits->size() != exp_trk.size()) ok = false;
    if (event.trksegs && event.trksegs->size() != exp_trk.size()) ok = false;

    auto clusters = event.GetCaloClusters();
    if (event.caloclusters) {
      ++n_calo_checked;
      if (clusters.size() != exp_calo.size() || event.caloclusters->size() != exp_calo.size()) ok = false;
      else for (size_t i = 0; i < exp_calo.size(); ++i) {
        if (clusters[i].calocluster->energyDep_ != exp_calo[i]) ok = false;
        if (event.caloclusters->at(i).energyDep_ != exp_calo[i]) ok = false;
      }
    }

    ++n_checked;
    if (reject_before_keep) ++n_reject_before_keep;
    if (calo_rbk) ++n_calo_rbk;
    if (!ok) { ++n_fail; if (n_fail < 10) std::cout << "FAIL: event " << i_event << std::endl; }
  }
  std::cout << "checked " << n_checked << " events (" << n_reject_before_keep
            << " with a rejected track before a kept one, " << n_calo_checked << " with calo clusters, "
            << n_calo_rbk << " with a rejected cluster before a kept one), "
            << n_fail << " failures" << std::endl;
  return n_fail;
}
