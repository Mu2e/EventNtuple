#! /usr/bin/env python
import sys
sys.path.append("../../utils/pyutils")

import pyimport as evn
import pyvector as vec
import pyplot as plot
import pyprint as prnt

import awkward as ak
import argparse

def example_multifiles(filelist):
  """ simple test function to run some of the utils """
  
  # import the files
  myevn = evn.Import(None, "EventNtuple", "ntuple")


  # import code and extract branch
  treename = 'trksegs'
  branchname = 'time'
  surface_id = 1 # tracker middle
  branch = myevn.ImportTreeFromFileList(filepath, treename)

  # find fit at chosen ID
  trkent = myevn.SelectSurfaceID(branch, treename, surface_id)

  # make 1D plot
  plotter = plot.Plot()
  plotter.Plot1D(
    array=flatarraytime,
    nbins=100,
    xmin=450,
    xmax=1695,
    title="Example 1D histogram",
    xlabel="Fit time at Trk Ent [ns]",
    ylabel="Events per bin",
    out_path='h1_time_merged.png',
    stat_box=True,
    error_bars=True
  )
  
def main(args):
  example_multifiles(args.filelist)
  
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='command arguments', formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("--filelist", type=str, default="/exp/mu2e/data/users/sophie/ensembles/MDS1/file.list", help="filelist")
    args = parser.parse_args()
    (args) = parser.parse_args()
    main(args)
