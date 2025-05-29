#! /usr/bin/env python
import pyimport as evn
import pyvector as vec
import pyplot as plot
import pyprint as prnt
from pyselect import Select as slct

import awkward as ak
def main():
  """ simple test function to run some of the utils """

  # import the files
  myevn = evn.Import("/exp/mu2e/data/users/sophie/ensembles/MDS1/MDS1av0.root", "EventNtuple", "ntuple")

  # import code and extract branch
  ntuple = myevn.ImportTree()

  # make a pyselect object
  mysel = slct()
  
  # check if it is an electron
  is_elec = mysel.isElectron(nutple)

  # import branches associated with trk fits
  trksegs = myevn.ImportBranches(ntuple,['trksegs','trksegpars_lh'])
  
  # check if fit is going down stream
  is_down = mysel.isDownstream(trksegs)

  # is trk middle
  trkent_mask = (trksegs['trksegs']['sid']==1) 
  
  # is in time
  trksegs_mask = (trksegs['trksegs']['time'] > 640) & (trksegs['trksegs']['time'] < 1650)
  
  # check trk pars
  trkpars_mask = (trksegs['trksegpars_lh']['t0err'] < 0.9) & (trksegs['trksegpars_lh']['maxr'] < 680) & (trksegs['trksegpars_lh']['maxr'] > 450) 
  
  
  # these are deprecated cuts
  oldtrkpar_mask = (trksegs['trksegpars_lh']['tanDip'] > 0.5) & (trksegs['trksegpars_lh']['tanDip'] < 1.0) & (trksegs['trksegpars_lh']['d0'] > -100) & (trksegs['trksegpars_lh']['d0'] < 100)
  
  # check trk quality
  trkqual = ntuple["trkqual"] 
  mytrkqual = trkqual.arrays(library='ak')
  trkqual_mask = (mytrkqual['trkqual.result']> 0.2)
  
  # check for crv coincidences 
  crvs = ntuple["crvcoincs"]
  mycrvs = crvs.arrays(library='ak')
  crv_mask = mysel.hasTrkCrvCoincs( trksegs, mycrvs, 150)
  
  # apply joint mask
  mytrksegs = trksegs['trksegs'].mask[(is_elec) & (is_down) & (trkent_mask)  & (trksegs_mask)   & (crv_mask) &(oldtrkpar_mask) & (active_mask) & (trkqual_mask) & (trkpars_mask)  ]

  # plot time before and after cuts:
  myhist = plot.Plot()
  flatarraytime = ak.flatten(trksegs['trksegs']['time'], axis=None)
  flatarraycut = ak.flatten(mytrksegs['time'], axis=None)
  dictarrays = { "no cut" : flatarraytime, "with cut" : flatarraycut }
  myhist.Plot1DOverlay(dictarrays, 100, 450, 1695, "Mu2e Example", "fit time at Trk Ent [ns]", "#events per bin", 'timecut.pdf', 'best', 300,False, True, True)
  
  # plot the momentum before and after the cuts
  myvect = vec.Vector()
  electrksegs = trkentall['trksegs'].mask[(is_elec) & (is_down)]
  vector_all = myvect.GetVectorXYZFromLeaf(electrksegs, 'mom')
  magnitude_all = myvect.Mag(vector_all)
  
  vector_cut = myvect.GetVectorXYZFromLeaf(mytrksegs, 'mom')
  magnitude_cut = myvect.Mag(vector_cut)
  
  flatarraymom_all = ak.flatten(magnitude_all, axis=None)
  flatarraymom_cut = ak.flatten(magnitude_cut, axis=None)
  
  myhist.Plot1D(flatarraymom_cut  , None, 100, 95, 115, "Mu2e Example", "fit mom at Trk Ent [MeV/c]", "#events per bin", 'black', 'best', 'momcut.pdf', 300, True, False, True, False, True, True, True)
  
  dictarrays = { "all dem" : flatarraymom_all, "dem + trkcuts" : flatarraymom_cut }
  myhist.Plot1DOverlay(dictarrays, 100, 95,115, "Mu2e Example", "fit mom at Trk Ent [ns]", "#events per bin", 'momcutcompare.pdf', 'best', 300,False, True, True)
  
  
  


  
if __name__ == "__main__":
    main()
