#!/usr/bin/env python3
"""Example: the track-CRV association branches of the EventNtuple.

For a track branch <trk> (e.g. "trk") configured with trkCrvMatchTags, the ntuple holds, per track:
  <trk>crv        one TrkCrvInfo: counts, bestgeom / besttime indices into <trk>crvcands, reference times
  <trk>crvxings   TrkCrvXingInfo per CRV sector crossing of the extrapolation (inbounds), or, for a straight-line track
                  that crosses no sector, per projection onto a sector's extended plane (not inbounds), with the 4-layer probe
  <trk>crvcands   TrkCrvCandInfo per associated CRV cluster: residuals, chi2, flags, and crvcoinc, the index into crvcoincs
and, with MC, <trk>crvcandsmc (parallel to crvcands), <trk>crvxingsmc (parallel to crvxings) and <trk>crvtruexingsmc.

This script shows the common operations, all vectorised with awkward (no event loops):
  1. the best candidate of each track (bestgeom), and its residuals;
  2. the join from a candidate to its cluster in crvcoincs (PEs, layers);
  3. a simple veto-style selection: tracks with a geometric candidate inside |dtxing| < 20 ns;
     and, for fits that do not reach the CRV (LoopHelix, time only), the best time candidate and a dtref window;
  4. the layer probe: per-layer pulse-finding fraction on crossed sectors;
  5. with MC: the truth relation of the best geometric and best time candidates (purity).

Run in the Mu2e analysis environment (mu2einit; pyenv ana) or any env with uproot, awkward, numpy, matplotlib:
  python3 trkcrv_example.py nts.root [--trk trk] [--entries 10000] [--plots outdir]
"""
import argparse
import os

import awkward as ak
import numpy as np
import uproot

REL_NAMES = ["no MC", "unrelated", "same", "daughter", "mother", "sibling", "udaughter", "umother", "usibling"]  # rel + 2, or no MC


def load(path, trk, entries):
    """Read the association branches and crvcoincs into one awkward array.

    Per-track branches (<trk>crv, crvcoincs) are split: their leaves are fields 'branch.leaf'. The per-candidate and
    per-crossing branches are vectors of vectors, which ROOT does not split: each is one field holding records."""
    tree = uproot.open(path)["EventNtuple/ntuple"]
    wanted = {trk + "crv", trk + "crvxings", trk + "crvcands", "crvcoincs", trk + "crvcandsmc", trk + "crvtruexingsmc"}
    if trk + "crvcands" not in tree.keys(recursive=False):
        raise SystemExit("no %scrvcands branch: was the ntuple made with trkCrvMatchTags for this branch?" % trk)
    keys = [b.name for b in tree.itervalues(recursive=True) if b.name.split(".")[0] in wanted and len(b.branches) == 0]
    arrays = tree.arrays(keys, entry_stop=entries)
    return arrays, (trk + "crvcandsmc") in keys


def leaf(arrays, branch, name):
    """The leaf 'name' of 'branch', split (field 'branch.name') or not (record field of 'branch')."""
    key = branch + "." + name
    if key in arrays.fields:
        return arrays[key]
    if branch in arrays.fields:
        return arrays[branch][name]
    raise KeyError(key)


def pick(per_cand, index):
    """per_cand: [event][track][cand]; index: [event][track], -1 = none. Returns [event][track], None where index < 0."""
    safe = ak.mask(index, index >= 0)
    return ak.firsts(per_cand[ak.singletons(safe, axis=1)], axis=2)


def per_track_coincs(coinc_leaf, ntrk_like):
    """Repeat the event's crvcoincs leaf ([event][coinc]) for every track: [event][track][coinc]."""
    counts = ak.num(ntrk_like, axis=1)
    event_of_track = np.repeat(np.arange(len(counts)), ak.to_numpy(counts))
    return ak.unflatten(coinc_leaf[event_of_track], counts)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("ntuple")
    ap.add_argument("--trk", default="trk", help="track branch name (default trk)")
    ap.add_argument("--entries", type=int, default=None, help="read only the first N events")
    ap.add_argument("--plots", default=None, help="directory for PNG plots (needs matplotlib)")
    args = ap.parse_args()

    a, has_mc = load(args.ntuple, args.trk, args.entries)
    crv, cands, xings = args.trk + "crv", args.trk + "crvcands", args.trk + "crvxings"

    # --- 1. best candidate per track ---------------------------------------------------------------------------
    matched = leaf(a, crv, "matched")
    bestgeom = leaf(a, crv, "bestgeom")
    ntracks = int(ak.sum(ak.num(matched, axis=1)))
    inbounds = leaf(a, xings, "inbounds")                             # [event][track][xing]
    has_xing = ak.any(inbounds, axis=2)                               # crosses a CRV sector (not nxings > 0: projections)
    has_proj = ak.any(~inbounds, axis=2)                              # a line track that crosses none, projected
    has_geom = bestgeom >= 0
    print("tracks %d, with a TrkCrvMatch record %d, crossing the CRV %d, projected only %d, with a geometric candidate %d"
          % (ntracks, ak.sum(matched), ak.sum(has_xing), ak.sum(has_proj), ak.sum(has_geom)))
    best_dv = pick(leaf(a, cands, "dv"), bestgeom)
    best_dtxing = pick(leaf(a, cands, "dtxing"), bestgeom)
    best_chi2 = pick(leaf(a, cands, "chi2"), bestgeom)
    best_ndof = pick(leaf(a, cands, "ndof"), bestgeom)
    best_sector = pick(leaf(a, cands, "sector"), bestgeom)
    dv = ak.to_numpy(ak.flatten(ak.drop_none(best_dv), axis=None))
    if len(dv):
        print("best geometric candidate: median dv %.1f mm, median dtxing %.2f ns, median chi2/ndof %.2f"
              % (np.median(dv), np.median(ak.to_numpy(ak.flatten(ak.drop_none(best_dtxing), axis=None))),
                 np.median(ak.to_numpy(ak.flatten(ak.drop_none(best_chi2 / best_ndof), axis=None)))))

    # --- 2. join candidates to crvcoincs -----------------------------------------------------------------------
    if "crvcoincs.PEs" in a.fields:
        coinc_idx = leaf(a, cands, "crvcoinc")                       # [event][track][cand]
        pes_by_trk = per_track_coincs(a["crvcoincs.PEs"], bestgeom)    # [event][track][coinc]
        cand_pes = pes_by_trk[ak.mask(coinc_idx, coinc_idx >= 0)]    # [event][track][cand], None if not in crvcoincs
        best_pes = pick(cand_pes, bestgeom)
        pes = ak.to_numpy(ak.flatten(ak.drop_none(best_pes), axis=None))
        if len(pes):
            print("PEs of the best geometric candidate's cluster: median %.0f" % np.median(pes))
        n_unindexed = ak.sum(coinc_idx < 0)
        if n_unindexed:
            print("note: %d candidates are not in crvcoincs (the matcher used another cluster collection)" % n_unindexed)

    # --- 3. a veto-style selection -------------------------------------------------------------------------------
    in_time = leaf(a, cands, "geom") & (abs(leaf(a, cands, "dtxing")) < 20.0)
    vetoed = ak.any(in_time, axis=2)                                  # [event][track]
    print("tracks with a geometric candidate within |dtxing| < 20 ns: %d" % ak.sum(vetoed))

    # time only (e.g. LoopHelix fits): dtref = track reference time - cluster time, at every candidate
    besttime = leaf(a, crv, "besttime")
    dtref_best = ak.to_numpy(ak.flatten(ak.drop_none(pick(leaf(a, cands, "dtref"), besttime)), axis=None))
    print("tracks with a time candidate: %d" % ak.sum(besttime >= 0))
    if len(dtref_best):
        print("best time candidate: median dtref %.1f ns" % np.median(dtref_best))
    dtref = leaf(a, cands, "dtref")
    in_window = ak.any(leaf(a, cands, "time") & (dtref > 0) & (dtref < 150), axis=2)   # an example veto window
    print("tracks with a time candidate at 0 < dtref < 150 ns: %d" % ak.sum(in_window))

    # --- 4. layer probe -------------------------------------------------------------------------------------------
    pbar = leaf(a, xings, "probe_bar")[inbounds]                      # [event][track][xing][layer], crossed sectors only
    pdbar = leaf(a, xings, "probe_dbar")[inbounds]
    predicted = pbar >= 0
    found = predicted & (pdbar != 127)
    pred_per_layer = np.array([ak.sum(predicted[..., L]) for L in range(4)])
    found_per_layer = np.array([ak.sum(found[..., L]) for L in range(4)])
    if pred_per_layer.sum():
        frac = np.divide(found_per_layer, pred_per_layer, out=np.zeros(4), where=pred_per_layer > 0)
        print("layer probe, pulse found per predicted layer: " + ", ".join("L%d %.3f" % (L, f) for L, f in enumerate(frac)))

    # --- 5. truth of the best candidate ---------------------------------------------------------------------------
    rel_counts = None
    if has_mc:
        relbin = ak.where(leaf(a, args.trk + "crvcandsmc", "hasmc"), leaf(a, args.trk + "crvcandsmc", "rel") + 2, 0)
        r = ak.to_numpy(ak.flatten(ak.drop_none(pick(relbin, bestgeom)), axis=None))
        if len(r):
            rel_counts = np.bincount(r.astype(int), minlength=len(REL_NAMES))[:len(REL_NAMES)]
            print("best geometric candidate truth: " + ", ".join("%s %.3f" % (n, c / len(r)) for n, c in zip(REL_NAMES, rel_counts)))
        rt = ak.to_numpy(ak.flatten(ak.drop_none(pick(relbin, besttime)), axis=None))
        if len(rt):
            counts = np.bincount(rt.astype(int), minlength=len(REL_NAMES))[:len(REL_NAMES)]
            print("best time candidate truth: " + ", ".join("%s %.3f" % (n, c / len(rt)) for n, c in zip(REL_NAMES, counts)))

    if args.plots:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
        os.makedirs(args.plots, exist_ok=True)
        sectors = ak.to_numpy(ak.flatten(ak.drop_none(best_sector), axis=None))
        fig, ax = plt.subplots(1, 3, figsize=(15, 4))
        for s in np.unique(sectors):
            ax[0].hist(dv[sectors == s], bins=100, range=(-200, 200), histtype="step", label="sector %d" % s)
        ax[0].set_xlabel("dv, best geometric candidate [mm]")
        ax[0].legend(fontsize=7)
        ax[1].hist(ak.to_numpy(ak.flatten(ak.drop_none(best_dtxing), axis=None)), bins=100, range=(-50, 50), histtype="step")
        ax[1].set_xlabel("dtxing, best geometric candidate [ns]")
        if pred_per_layer.sum():
            ax[2].bar(range(4), frac)
            ax[2].set_xlabel("CRV layer")
            ax[2].set_ylabel("pulse found / predicted")
        fig.tight_layout()
        out = os.path.join(args.plots, "trkcrv_example.png")
        fig.savefig(out, dpi=120)
        print("wrote " + out)


if __name__ == "__main__":
    main()
