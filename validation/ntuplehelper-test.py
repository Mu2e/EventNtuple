import ntuplehelper

nthelp = ntuplehelper.nthelper()

print("Testing a leaf from each branch")
print("=========")
nthelp.whatis(['evtinfomc.pbtime', 'evtinfo.event', 'hitcount.nbkg', 'tcnt.ndem', 'trkmcsim.pdg', 'trkcalohit.trkdepth', 'trkcalohitmc.nsim', 'trkhits.ptoca', 'trkhitsmc.edep', 'trkmats.tlen', 'crvsummary.totalPEs', 'crvsummarymc.sectorNumber', 'crvcoincs.time', 'crvcoincsmc.primaryE', 'crvcoincsmcplane.dir', 'trkmcsci.dp', 'trkmcssi.dp', 'trk.nactive', 'trkmc.ndigigood', 'trksegs.mom', 'trksegpars_lh.cx', 'trksegpars_ch.omega', 'trksegpars_kl.phi0', 'trkqual.result', 'helices.chi2xy', 'trksegsmc.path'])

print("Testing a few leaves from a single branch (should only print the branch description once)")
print("=========")
nthelp.whatis(['evtinfo.run', 'evtinfo.subrun', 'evtinfo.event']) # should only print the "evtinfo" description once

print("Testing wildcard")
print("=========")
nthelp.whatis(["trk.*", "trksegs.*", "caloclusters.*"])

print("Testing a few errors")
print("=========")
nthelp.whatis(['dem.lha', 'fff.gap', 'fff.ddd'])
