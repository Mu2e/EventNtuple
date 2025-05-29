#! /usr/bin/env python
import sys
sys.path.append("../../utils/pyutils")

import pyimport as evn
import pyvector as vec
import pyplot as plot
import pyprint as prnt
import pyselect as slct

import awkward as ak
import argparse

def example_plotting(filename):
  """ Simple demo function to run some of the utils """
  
  # Import the files
  test_evn = evn.Import(str(filename), "EventNtuple", "ntuple")

  # Import code and extract branch
  treename = 'trksegs'
  branchname = 'time'
  surface_id = 1 # tracker middle
  ntuple = test_evn.ImportTree()
  branches = test_evn.ImportBranches(ntuple,[str(treename)])
  
  # Find fit at chosen ID
  selector = slct.Select()
  trkent = selector.SelectSurfaceID(branches, treename, surface_id)
  
  # Print out the first event
  printer = prnt.Print()
  # Print PrintNEvents help
  help(printer.PrintNEvents)
  printer.PrintNEvents(branches, n_events=1)
  
  # Make 1D plot from flattened array
  flatarraytime = ak.flatten(trkent[str(branchname)], axis=None)
  plotter = plot.Plot()

  # Print Plot1D help (press "q" to exit)
  help(plotter.Plot1D)

  # Make plot
  plotter.Plot1D(
    array=flatarraytime,
    nbins=100,
    xmin=450,
    xmax=1695,
    title="Example 1D histogram",
    xlabel="Fit time at Trk Ent [ns]",
    ylabel="Events per bin",
    out_path='h1_time.png',
    stat_box=True,
    error_bars=True
  )
  
  # Access vectors
  vector = vec.Vector()
  vecbranchname = 'mom'
  trkentall = selector.SelectSurfaceID(branches, treename, surface_id)
  vector_test = vector.GetVectorXYZFromLeaf(trkentall, vecbranchname)
  magnitude = vector.Mag(vector_test)

  # make 1D plot of magnitudes
  flatarraymom = ak.flatten(magnitude, axis=None)
  
  # Make plot
  plotter.Plot1D(
    array=flatarraymom,
    nbins=100,
    xmin=0,
    xmax=300,
    title="Example 1D histogram",
    xlabel="Fit mom at Trk Ent [MeV/c]",
    ylabel="Events per bin",
    out_path='h1_mom.png',
    stat_box=True,
    error_bars=True,
    log_y=True
  )
  
  
  # Print Plot2D help (press "q" to exit)
  help(plotter.Plot2D)

  # Make 2D plot
  plotter.Plot2D(
    x=flatarraymom,
    y=flatarraytime,
    nbins_x=100,
    xmin=95,
    xmax=115,
    nbins_y=100,
    ymin=450,
    ymax=1650, 
    title="Example 2D histogram",
    xlabel="Fit mom at Trk Ent [MeV/c]",
    ylabel="Fit time at Trk Ent [ns]",
    out_path='h2_timevmom.png'
  )
  
def main(args):
  example_plotting(args.filename)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='command arguments', formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("--filename", type=str, default="/exp/mu2e/data/users/sophie/ensembles/MDS1/MDS1av0.root", help="filename")
    args = parser.parse_args()
    (args) = parser.parse_args()
    main(args)
