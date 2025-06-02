#! /usr/bin/env python
import sys
sys.path.append("../../utils/pyutils")

import pyimport as evn
import pyvector as vec
import pyplot as plot
import pyprint as prnt
from pyselect import Select as slct

import awkward as ak
import argparse
def example_multicuts(filename):
  """ simple test function to run some of the utils """

  # import the files
  test_evn = evn.Import(str(filename), "EventNtuple", "ntuple")

  # import code and extract branch
  ntuple = test_evn.ImportTree()

  # make a pyselect object
  mysel = slct()

  # import branches associated with trk fits
  trksegs = test_evn.ImportBranches(ntuple,['trksegs','trksegpars_lh'])

  # check if it is an electron going downstream
  is_elec = mysel.isElectron(ntuple)
  is_down = mysel.isDownstream(trksegs)
   
  # active hits
  active_mask = mysel.SelectHits(ntuple, 20)
  
  # check trk quality
  trkqual_mask = mysel.SelectTrkQual(ntuple, 0.2)

  # check for crv coincidences 
  crv_mask = mysel.hasTrkCrvCoincs( trksegs, ntuple, 150)
  
  # set of trkseg cuts
  treenames = [ 'trksegs', 'trksegs', 'trksegpars_lh', 'trksegpars_lh', 'trksegpars_lh', 'trksegpars_lh']
  leaves = [ 'sid', 'time', 't0err','maxr','tanDip','d0']
  equals = [True, False, False, False, False, False]
  v1s = [1, 640, 0, 450, 0.5, -100]
  v2s = [None, 1650, 0.9, 680, 1.0, 100]
  
  # make a list of masks
  trkseg_mask_list = mysel.MakeMaskList(trksegs, treenames, leaves, equals, v1s, v2s)

  # FIXME apply joint mask --> can we make this tidier? like a loop or somethin?
  mytrksegs = trksegs['trksegs'].mask[(is_elec) & (is_down) & (trkqual_mask) & (crv_mask) & (active_mask) & (trkseg_mask_list[0]) & (trkseg_mask_list[1]) &  (trkseg_mask_list[2]) & (trkseg_mask_list[3]) & (trkseg_mask_list[4]) & (trkseg_mask_list[5]) ]
  
  # make some plots to compare before/after cuts
  myhist = plot.Plot()
  
  # plot the momentum before and after the cuts
  myvect = vec.Vector()
  electrksegs = trksegs['trksegs'].mask[(is_elec) & (is_down)]
  vector_all = myvect.GetVectorXYZFromLeaf(electrksegs, 'mom')
  magnitude_all = myvect.Mag(vector_all)
  
  vector_cut = myvect.GetVectorXYZFromLeaf(mytrksegs, 'mom')
  magnitude_cut = myvect.Mag(vector_cut)
  
  flatarraymom_all = ak.flatten(magnitude_all, axis=None)
  flatarraymom_cut = ak.flatten(magnitude_cut, axis=None)
  
  myhist.Plot1D(flatarraymom_cut  , None, 100, 95, 115, "Mu2e Example", "fit mom at Trk Ent [MeV/c]", "#events per bin", 'black', 'best', 'momcut.pdf', 300, True, False, True, False, True, True, True)
  
  dictarrays = { "all dem" : flatarraymom_all, "dem + trkcuts" : flatarraymom_cut }
  myhist.Plot1DOverlay(dictarrays, 100, 95,115, "Mu2e Example", "fit mom at Trk Ent [ns]", "#events per bin", 'momcutcompare.pdf', 'best', 300,False, True, True)

def main(args):
  example_multicuts(args.filename)
  
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='command arguments', formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("--filename", type=str, default="/exp/mu2e/data/users/sophie/ensembles/MDS1/MDS1av0.root", help="filename")
    args = parser.parse_args()
    (args) = parser.parse_args()
    main(args)
