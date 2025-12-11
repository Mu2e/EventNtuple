//
// create_val_file_rooutil.C
// -- this macro creates a histogram for every leaf in the EventNtuple using the RooUtil interface
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
// TODO: can add common_cuts.hh here

#include "TH1F.h"
using namespace rooutil;

void create_val_file_rooutil(std::string filename, std::string outfilename) {

  RooUtil util(filename);

  TFile* file = new TFile(outfilename.c_str(), "RECREATE");

  TH1F* h_evtinfo_event = new TH1F("h_evtinfo_event", "", 100,0,2000);
  TH1F* h_evtinfo_subrun = new TH1F("h_evtinfo_subrun", "", 100,0,2000);
  TH1F* h_evtinfo_run = new TH1F("h_evtinfo_run", "", 100,0,2000);
  TH1F* h_evtinfo_nprotons = new TH1F("h_evtinfo_nprotons", "", 100,0,1000);
  TH1F* h_evtinfo_pbtime = new TH1F("h_evtinfo_pbtime", "", 100,-500,500);
  TH1F* h_evtinfo_pbterr = new TH1F("h_evtinfo_pbterr", "", 100,-1,1);

  TH1F* h_evtinfomc_nprotons = new TH1F("h_evtinfomc_nprotons", "", 100,0,1000);
  TH1F* h_evtinfomc_pbtime = new TH1F("h_evtinfomc_pbtime", "", 100,-500,500);

  TH1F* h_trk_status = new TH1F("h_trk_status", "", 100,0,100);
  TH1F* h_trk_goodfit = new TH1F("h_trk_goodfit", "", 100,0,100);
  TH1F* h_trk_seedalg = new TH1F("h_trk_seedalg", "", 100,0,100);
  TH1F* h_trk_fitalg = new TH1F("h_trk_fitalg", "", 100,0,100);
  TH1F* h_trk_pdg = new TH1F("h_trk_pdg", "", 30,-15,15);
  TH1F* h_trk_nhits = new TH1F("h_trk_nhits", "", 100,0,100);
  TH1F* h_trk_ndof = new TH1F("h_trk_ndof", "", 150,0,150);
  TH1F* h_trk_nactive = new TH1F("h_trk_nactive", "", 100,0,100);
  TH1F* h_trk_ndouble = new TH1F("h_trk_ndouble", "", 50,0,50);
  TH1F* h_trk_ndactive = new TH1F("h_trk_ndactive", "", 50,0,50);
  TH1F* h_trk_nplanes = new TH1F("h_trk_nplanes", "", 40,0,40);
  TH1F* h_trk_planespan = new TH1F("h_trk_planespan", "", 40,0,40);
  TH1F* h_trk_nnullambig = new TH1F("h_trk_nnullambig", "", 50,0,50);
  TH1F* h_trk_nmat = new TH1F("h_trk_nmat", "", 200,0,200);
  TH1F* h_trk_nmatactive = new TH1F("h_trk_nmatactive", "", 200,0,200);
  TH1F* h_trk_nesel = new TH1F("h_trk_nesel", "", 100,0,100);
  TH1F* h_trk_nrsel = new TH1F("h_trk_nrsel", "", 100,0,100);
  TH1F* h_trk_ntsel = new TH1F("h_trk_ntsel", "", 100,0,100);
  TH1F* h_trk_nbkg = new TH1F("h_trk_nbkg", "", 100,0,100);
  TH1F* h_trk_nsel = new TH1F("h_trk_nsel", "", 100,0,100);
  TH1F* h_trk_nseg = new TH1F("h_trk_nseg", "", 200,0,200);
  TH1F* h_trk_nipaup = new TH1F("h_trk_nipaup", "", 10,0,10);
  TH1F* h_trk_nipadown = new TH1F("h_trk_nipadown", "", 10,0,10);
  TH1F* h_trk_nstup = new TH1F("h_trk_nstup", "", 20,0,20);
  TH1F* h_trk_nstdown = new TH1F("h_trk_nstdown", "", 20,0,20);
  TH1F* h_trk_tsdainter = new TH1F("h_trk_tsdainter", "", 10,0,10);
  TH1F* h_trk_opainter = new TH1F("h_trk_opainter", "", 10,0,10);
  TH1F* h_trk_firststinter = new TH1F("h_trk_firststinter", "", 200,-100,100);
  TH1F* h_trk_chisq = new TH1F("h_trk_chisq", "", 100,0,500);
  TH1F* h_trk_fitcon = new TH1F("h_trk_fitcon", "", 100,0,1);
  TH1F* h_trk_radlen = new TH1F("h_trk_radlen", "", 100,0,1);
  TH1F* h_trk_firsthit = new TH1F("h_trk_firsthit", "", 100,0,500);
  TH1F* h_trk_lasthit = new TH1F("h_trk_lasthit", "", 100,0,500);
  TH1F* h_trk_maxgap = new TH1F("h_trk_maxgap", "", 100,0,1);
  TH1F* h_trk_avggap = new TH1F("h_trk_avggap", "", 100,0,1);
  TH1F* h_trk_avgedep = new TH1F("h_trk_avgedep", "", 100,0,100);

  TH1F* h_trksegs_mom_x = new TH1F("h_trksegs_mom_x", "", 400,-200,200);
  TH1F* h_trksegs_mom_y = new TH1F("h_trksegs_mom_y", "", 400,-200,200);
  TH1F* h_trksegs_mom_z = new TH1F("h_trksegs_mom_z", "", 400,-200,200);
  TH1F* h_trksegs_mom_mag = new TH1F("h_trksegs_mom_mag", "", 100,0,200);
  TH1F* h_trksegs_pos_x = new TH1F("h_trksegs_pos_x", "", 100,-1000,1000);
  TH1F* h_trksegs_pos_y = new TH1F("h_trksegs_pos_y", "", 100,-1000,1000);
  TH1F* h_trksegs_pos_z = new TH1F("h_trksegs_pos_z", "", 100,-1000,1000);
  TH1F* h_trksegs_time = new TH1F("h_trksegs_time", "", 200,0,2000);
  TH1F* h_trksegs_dmom = new TH1F("h_trksegs_dmom", "", 100,0,1);
  TH1F* h_trksegs_momerr = new TH1F("h_trksegs_momerr", "", 100,0,5);
  TH1F* h_trksegs_inbounds = new TH1F("h_trksegs_inbounds", "", 100,0,5);
  TH1F* h_trksegs_gap = new TH1F("h_trksegs_gap", "", 100,0,5);
  TH1F* h_trksegs_sid = new TH1F("h_trksegs_sid", "", 200,0,200);
  TH1F* h_trksegs_sindex = new TH1F("h_trksegs_sindex", "", 50,0,50);

  TH1F* h_trkmc_valid = new TH1F("h_trkmc_valid", "",100,0,10);
  TH1F* h_trkmc_ndigi = new TH1F("h_trkmc_ndigi", "",100,0,100);
  TH1F* h_trkmc_ndigigood = new TH1F("h_trkmc_ndigigood", "",100,0,100);
  TH1F* h_trkmc_nhits = new TH1F("h_trkmc_nhits", "",100,0,100);
  TH1F* h_trkmc_nactive = new TH1F("h_trkmc_nactive", "",100,0,100);
  TH1F* h_trkmc_nambig = new TH1F("h_trkmc_nambig", "",100,0,100);
  TH1F* h_trkmc_nipaup = new TH1F("h_trkmc_nipaup", "",100,0,100);
  TH1F* h_trkmc_nipadown = new TH1F("h_trkmc_nipadown", "",100,0,100);
  TH1F* h_trkmc_nstup = new TH1F("h_trkmc_nstup", "",100,0,100);
  TH1F* h_trkmc_nstdown = new TH1F("h_trkmc_nstdown", "",100,0,100);
  TH1F* h_trkmc_maxr = new TH1F("h_trkmc_maxr", "",100,0,1000);
  TH1F* h_trkmc_rad = new TH1F("h_trkmc_rad", "",200,-1000,1000);
  TH1F* h_trkmc_lam = new TH1F("h_trkmc_lam", "",200,-1000,1000);
  TH1F* h_trkmc_cx = new TH1F("h_trkmc_cx", "",100,-500,500);
  TH1F* h_trkmc_cy = new TH1F("h_trkmc_cy", "",100,-500,500);
  TH1F* h_trkmc_phi0 = new TH1F("h_trkmc_phi0", "",100,0,10);
  TH1F* h_trkmc_t0 = new TH1F("h_trkmc_t0", "",200,0,2000);

  TH1F* h_trksegsmc_sid = new TH1F("h_trksegsmc_sid", "", 200,0,200);
  TH1F* h_trksegsmc_sindex = new TH1F("h_trksegsmc_sindex", "", 50,0,50);
  TH1F* h_trksegsmc_time = new TH1F("h_trksegsmc_time", "", 100,0,100);
  TH1F* h_trksegsmc_edep = new TH1F("h_trksegsmc_edep", "", 100,0,100);
  TH1F* h_trksegsmc_path = new TH1F("h_trksegsmc_path", "", 200,0,200);
  TH1F* h_trksegsmc_pos_x = new TH1F("h_trksegsmc_pos_x", "", 200,-1000,1000);
  TH1F* h_trksegsmc_pos_y = new TH1F("h_trksegsmc_pos_y", "", 200,-1000,1000);
  TH1F* h_trksegsmc_pos_z = new TH1F("h_trksegsmc_pos_z", "", 200,-1000,1000);
  TH1F* h_trksegsmc_mom_x = new TH1F("h_trksegsmc_mom_x", "", 400,-200,200);
  TH1F* h_trksegsmc_mom_y = new TH1F("h_trksegsmc_mom_y", "", 400,-200,200);
  TH1F* h_trksegsmc_mom_z = new TH1F("h_trksegsmc_mom_z", "", 400,-200,200);

  TH1F* h_trkcalohit_active = new TH1F("h_trkcalohit_active", "", 100,0,100);
  TH1F* h_trkcalohit_did = new TH1F("h_trkcalohit_did", "", 10,-5,5);
  TH1F* h_trkcalohit_poca_x = new TH1F("h_trkcalohit_poca_x", "", 200,-200,200);
  TH1F* h_trkcalohit_poca_y = new TH1F("h_trkcalohit_poca_y", "", 200,-200,200);
  TH1F* h_trkcalohit_poca_z = new TH1F("h_trkcalohit_poca_z", "", 200,-200,200);
  TH1F* h_trkcalohit_mom_x = new TH1F("h_trkcalohit_mom_x", "", 400,-200,200);
  TH1F* h_trkcalohit_mom_y = new TH1F("h_trkcalohit_mom_y", "", 400,-200,200);
  TH1F* h_trkcalohit_mom_z = new TH1F("h_trkcalohit_mom_z", "", 400,-200,200);
  TH1F* h_trkcalohit_cdepth = new TH1F("h_trkcalohit_cdepth", "", 100,-200,200);
  TH1F* h_trkcalohit_trkdepth = new TH1F("h_trkcalohit_trkdepth", "", 100,-200,200);
  TH1F* h_trkcalohit_dphidot = new TH1F("h_trkcalohit_dphidot", "", 100,-50,50);
  TH1F* h_trkcalohit_doca = new TH1F("h_trkcalohit_doca", "", 200,-100,100);
  TH1F* h_trkcalohit_dt = new TH1F("h_trkcalohit_dt", "", 100,-10,10);
  TH1F* h_trkcalohit_ptoca = new TH1F("h_trkcalohit_ptoca", "", 100,-500,500);
  TH1F* h_trkcalohit_tocavar = new TH1F("h_trkcalohit_tocavar", "", 100,-10,10);
  TH1F* h_trkcalohit_tresid = new TH1F("h_trkcalohit_tresid", "", 100,-20,20);
  TH1F* h_trkcalohit_tresidmvar = new TH1F("h_trkcalohit_tresidmvar", "", 100,-10,10);
  TH1F* h_trkcalohit_tresidpvar = new TH1F("h_trkcalohit_tresidpvar", "", 100,-10,10);
  TH1F* h_trkcalohit_ctime = new TH1F("h_trkcalohit_ctime", "", 210,-100,2000);
  TH1F* h_trkcalohit_ctimeerr = new TH1F("h_trkcalohit_ctimeerr", "", 100,-10,10);
  TH1F* h_trkcalohit_csize = new TH1F("h_trkcalohit_csize", "", 40,-20,20);
  TH1F* h_trkcalohit_edep = new TH1F("h_trkcalohit_edep", "", 100,-10,200);
  TH1F* h_trkcalohit_edeperr = new TH1F("h_trkcalohit_edeperr", "", 100,-10,10);

  TH1F* h_trkcalohitmc_nsim = new TH1F("h_trkcalohitmc_nsim", "", 100,0,100);
  TH1F* h_trkcalohitmc_etot = new TH1F("h_trkcalohitmc_etot", "", 200,0,200);
  TH1F* h_trkcalohitmc_tavg = new TH1F("h_trkcalohitmc_tavg", "", 200,0,2000);
  TH1F* h_trkcalohitmc_eprimary = new TH1F("h_trkcalohitmc_eprimary", "", 200,0,200);
  TH1F* h_trkcalohitmc_tprimary = new TH1F("h_trkcalohitmc_tprimary", "", 200,0,2000);
  TH1F* h_trkcalohitmc_prel = new TH1F("h_trkcalohitmc_prel", "", 20,-10,10);

  TH1F* h_crvcoincs_sectorType = new TH1F("h_crvcoincs_sectorType", "", 100,0,100);
  TH1F* h_crvcoincs_pos_x = new TH1F("h_crvcoincs_pos_x", "", 100,-200,200);
  TH1F* h_crvcoincs_pos_y = new TH1F("h_crvcoincs_pos_y", "", 100,-200,200);
  TH1F* h_crvcoincs_pos_z = new TH1F("h_crvcoincs_pos_z", "", 100,-200,200);
  TH1F* h_crvcoincs_timeStart = new TH1F("h_crvcoincs_timeStart", "", 200,0,2000);
  TH1F* h_crvcoincs_timeEnd = new TH1F("h_crvcoincs_timeEnd", "", 200,0,2000);
  TH1F* h_crvcoincs_time = new TH1F("h_crvcoincs_time", "", 200,0,2000);
  TH1F* h_crvcoincs_PEs = new TH1F("h_crvcoincs_PEs", "", 200,0,200);
  TH1F* h_crvcoincs_PEsPerLayer = new TH1F("h_crvcoincs_PEsPerLayer", "", 200,0,200);
  TH1F* h_crvcoincs_nHits = new TH1F("h_crvcoincs_nHits", "", 100,0,100);
  TH1F* h_crvcoincs_nLayers = new TH1F("h_crvcoincs_nLayers", "", 100,0,100);
  TH1F* h_crvcoincs_angle = new TH1F("h_crvcoincs_angle", "", 100,-5,5);

  TH1F* h_crvcoincsmc_valid = new TH1F("h_crvcoincsmc_valid", "", 100,0,100);
  TH1F* h_crvcoincsmc_pdgId = new TH1F("h_crvcoincsmc_pdgId", "", 100,-20,20);
  TH1F* h_crvcoincsmc_primaryPdgId = new TH1F("h_crvcoincsmc_primaryPdgId", "", 100,-20,20);
  TH1F* h_crvcoincsmc_primaryE = new TH1F("h_crvcoincsmc_primaryE", "", 100,0,5000);
  TH1F* h_crvcoincsmc_primary_x = new TH1F("h_crvcoincsmc_primary_x", "", 100,-200,200);
  TH1F* h_crvcoincsmc_primary_y = new TH1F("h_crvcoincsmc_primary_y", "", 100,-200,200);
  TH1F* h_crvcoincsmc_primary_z = new TH1F("h_crvcoincsmc_primary_z", "", 100,-200,200);
  TH1F* h_crvcoincsmc_parentPdgId = new TH1F("h_crvcoincsmc_parentPdgId", "", 100,-20,20);
  TH1F* h_crvcoincsmc_parentE = new TH1F("h_crvcoincsmc_parentE", "", 100,0,5000);
  TH1F* h_crvcoincsmc_parent_x = new TH1F("h_crvcoincsmc_parent_x", "", 100,-200,200);
  TH1F* h_crvcoincsmc_parent_y = new TH1F("h_crvcoincsmc_parent_y", "", 100,-200,200);
  TH1F* h_crvcoincsmc_parent_z = new TH1F("h_crvcoincsmc_parent_z", "", 100,-200,200);
  TH1F* h_crvcoincsmc_gparentPdgId = new TH1F("h_crvcoincsmc_gparentPdgId", "", 100,-20,20);
  TH1F* h_crvcoincsmc_gparentE = new TH1F("h_crvcoincsmc_gparentE", "", 100,0,5000);
  TH1F* h_crvcoincsmc_gparent_x = new TH1F("h_crvcoincsmc_gparent_x", "", 100,-200,200);
  TH1F* h_crvcoincsmc_gparent_y = new TH1F("h_crvcoincsmc_gparent_y", "", 100,-200,200);
  TH1F* h_crvcoincsmc_gparent_z = new TH1F("h_crvcoincsmc_gparent_z", "", 100,-200,200);
  TH1F* h_crvcoincsmc_pos_x = new TH1F("h_crvcoincsmc_pos_x", "", 100,-200,200);
  TH1F* h_crvcoincsmc_pos_y = new TH1F("h_crvcoincsmc_pos_y", "", 100,-200,200);
  TH1F* h_crvcoincsmc_pos_z = new TH1F("h_crvcoincsmc_pos_z", "", 100,-200,200);
  TH1F* h_crvcoincsmc_time = new TH1F("h_crvcoincsmc_time", "", 200,0,2000);
  TH1F* h_crvcoincsmc_depositedEnergy = new TH1F("h_crvcoincsmc_depositedEnergy", "", 100,0,100);

  TH1F* h_trkmcsim_valid = new TH1F("h_trkmcsim_valid", "", 100,0,100);
  TH1F* h_trkmcsim_id = new TH1F("h_trkmcsim_id", "", 100,0,100);
  TH1F* h_trkmcsim_nhits = new TH1F("h_trkmcsim_nhits", "", 100,0,100);
  TH1F* h_trkmcsim_nactive = new TH1F("h_trkmcsim_nactive", "", 100,0,100);
  TH1F* h_trkmcsim_rank = new TH1F("h_trkmcsim_rank", "", 100,0,100);
  TH1F* h_trkmcsim_pdg = new TH1F("h_trkmcsim_pdg", "", 200,-2500,2500);
  TH1F* h_trkmcsim_startCode = new TH1F("h_trkmcsim_startCode", "", 200,0,200);
  TH1F* h_trkmcsim_stopCode = new TH1F("h_trkmcsim_stopCode", "", 200,0,200);
  TH1F* h_trkmcsim_gen = new TH1F("h_trkmcsim_gen", "", 100,0,100);
  TH1F* h_trkmcsim_time = new TH1F("h_trkmcsim_time", "", 200,0,2000);
  TH1F* h_trkmcsim_index = new TH1F("h_trkmcsim_index", "", 100,0,100);
  TH1F* h_trkmcsim_mom_x = new TH1F("h_trkmcsim_mom_x", "", 100,-200,200);
  TH1F* h_trkmcsim_mom_y = new TH1F("h_trkmcsim_mom_y", "", 100,-200,200);
  TH1F* h_trkmcsim_mom_z = new TH1F("h_trkmcsim_mom_z", "", 100,-200,200);
  TH1F* h_trkmcsim_pos_x = new TH1F("h_trkmcsim_pos_x", "", 100,-500,500);
  TH1F* h_trkmcsim_pos_y = new TH1F("h_trkmcsim_pos_y", "", 100,-500,500);
  TH1F* h_trkmcsim_pos_z = new TH1F("h_trkmcsim_pos_z", "", 100,-500,500);
  TH1F* h_trkmcsim_endmom_x = new TH1F("h_trkmcsim_endmom_x", "", 100,-200,200);
  TH1F* h_trkmcsim_endmom_y = new TH1F("h_trkmcsim_endmom_y", "", 100,-200,200);
  TH1F* h_trkmcsim_endmom_z = new TH1F("h_trkmcsim_endmom_z", "", 100,-200,200);
  TH1F* h_trkmcsim_endpos_x = new TH1F("h_trkmcsim_endpos_x", "", 100,-500,500);
  TH1F* h_trkmcsim_endpos_y = new TH1F("h_trkmcsim_endpos_y", "", 100,-500,500);
  TH1F* h_trkmcsim_endpos_z = new TH1F("h_trkmcsim_endpos_z", "", 100,-500,500);
  TH1F* h_trkmcsim_prirel_rel = new TH1F("h_trkmcsim_prirel_rel", "", 20,-10,10);
  TH1F* h_trkmcsim_prirel_rem = new TH1F("h_trkmcsim_prirel_rem", "", 10,0,10);
  TH1F* h_trkmcsim_trkrel_rel = new TH1F("h_trkmcsim_trkrel_rel", "", 20,-10,10);
  TH1F* h_trkmcsim_trkrel_rem = new TH1F("h_trkmcsim_trkrel_rem", "", 10,0,10);

  TH1F* h_trkqual_valid = new TH1F("h_trkqual_valid", "", 100,0,100);
  TH1F* h_trkqual_result = new TH1F("h_trkqual_result", "", 100,0,1);

  TH1F* h_trksegpars_lh_maxr = new TH1F("h_trksegpars_lh_maxr", "", 100,0,500);
  TH1F* h_trksegpars_lh_d0 = new TH1F("h_trksegpars_lh_d0", "", 100,0,500);
  TH1F* h_trksegpars_lh_tanDip = new TH1F("h_trksegpars_lh_tanDip", "", 100,-5,5);
  TH1F* h_trksegpars_lh_rad = new TH1F("h_trksegpars_lh_rad", "", 100,-500,500);
  TH1F* h_trksegpars_lh_lam = new TH1F("h_trksegpars_lh_lam", "", 100,-500,500);
  TH1F* h_trksegpars_lh_cx = new TH1F("h_trksegpars_lh_cx", "", 100,-500,500);
  TH1F* h_trksegpars_lh_cy = new TH1F("h_trksegpars_lh_cy", "", 100,-500,500);
  TH1F* h_trksegpars_lh_phi0 = new TH1F("h_trksegpars_lh_phi0", "", 100,-5,50);
  TH1F* h_trksegpars_lh_t0 = new TH1F("h_trksegpars_lh_t0", "", 200,0,2000);
  TH1F* h_trksegpars_lh_raderr = new TH1F("h_trksegpars_lh_raderr", "", 100,0,100);
  TH1F* h_trksegpars_lh_lamerr = new TH1F("h_trksegpars_lh_lamerr", "", 100,0,100);
  TH1F* h_trksegpars_lh_cxerr = new TH1F("h_trksegpars_lh_cxerr", "", 100,0,100);
  TH1F* h_trksegpars_lh_cyerr = new TH1F("h_trksegpars_lh_cyerr", "", 100,0,100);
  TH1F* h_trksegpars_lh_phi0err = new TH1F("h_trksegpars_lh_phi0err", "", 100,0,100);
  TH1F* h_trksegpars_lh_t0err = new TH1F("h_trksegpars_lh_t0err", "", 100,0,100);

  TH1F* h_trksegpars_ch_d0 = new TH1F("h_trksegpars_ch_d0", "", 100,0,100);
  TH1F* h_trksegpars_ch_phi0 = new TH1F("h_trksegpars_ch_phi0", "", 100,0,100);
  TH1F* h_trksegpars_ch_omega = new TH1F("h_trksegpars_ch_omega", "", 100,0,100);
  TH1F* h_trksegpars_ch_z0 = new TH1F("h_trksegpars_ch_z0", "", 100,0,100);
  TH1F* h_trksegpars_ch_tanDip = new TH1F("h_trksegpars_ch_tanDip", "", 100,0,100);
  TH1F* h_trksegpars_ch_t0 = new TH1F("h_trksegpars_ch_t0", "", 100,0,100);
  TH1F* h_trksegpars_ch_d0err = new TH1F("h_trksegpars_ch_d0err", "", 100,0,100);
  TH1F* h_trksegpars_ch_phi0err = new TH1F("h_trksegpars_ch_phi0err", "", 100,0,100);
  TH1F* h_trksegpars_ch_omegaerr = new TH1F("h_trksegpars_ch_omegaerr", "", 100,0,100);
  TH1F* h_trksegpars_ch_z0err = new TH1F("h_trksegpars_ch_z0err", "", 100,0,100);
  TH1F* h_trksegpars_ch_tanDiperr = new TH1F("h_trksegpars_ch_tanDiperr", "", 100,0,100);
  TH1F* h_trksegpars_ch_t0err = new TH1F("h_trksegpars_ch_t0err", "", 100,0,100);
  TH1F* h_trksegpars_ch_maxr = new TH1F("h_trksegpars_ch_maxr", "", 100,0,100);

  TH1F* h_trksegpars_kl_d0 = new TH1F("h_trksegpars_kl_d0", "", 100,0,100);
  TH1F* h_trksegpars_kl_phi0 = new TH1F("h_trksegpars_kl_phi0", "", 100,0,100);
  TH1F* h_trksegpars_kl_z0 = new TH1F("h_trksegpars_kl_z0", "", 100,0,100);
  TH1F* h_trksegpars_kl_theta = new TH1F("h_trksegpars_kl_theta", "", 100,0,100);
  TH1F* h_trksegpars_kl_mom = new TH1F("h_trksegpars_kl_mom", "", 100,0,100);
  TH1F* h_trksegpars_kl_t0 = new TH1F("h_trksegpars_kl_t0", "", 100,0,100);
  TH1F* h_trksegpars_kl_d0err = new TH1F("h_trksegpars_kl_d0err", "", 100,0,100);
  TH1F* h_trksegpars_kl_phi0err = new TH1F("h_trksegpars_kl_phi0err", "", 100,0,100);
  TH1F* h_trksegpars_kl_z0err = new TH1F("h_trksegpars_kl_z0err", "", 100,0,100);
  TH1F* h_trksegpars_kl_thetaerr = new TH1F("h_trksegpars_kl_thetaerr", "", 100,0,100);
  TH1F* h_trksegpars_kl_momerr = new TH1F("h_trksegpars_kl_momerr", "", 100,0,100);
  TH1F* h_trksegpars_kl_t0err = new TH1F("h_trksegpars_kl_t0err", "", 100,0,100);

  TH1F* h_trkhits_plane = new TH1F("h_trkhits_plane", "", 36,0,36);
  TH1F* h_trkhits_panel = new TH1F("h_trkhits_panel", "", 6,0,6);
  TH1F* h_trkhits_layer = new TH1F("h_trkhits_layer", "", 2,0,2);
  TH1F* h_trkhits_straw = new TH1F("h_trkhits_straw", "", 100,0,100);
  TH1F* h_trkhits_state = new TH1F("h_trkhits_state", "", 100,-10,10);
  TH1F* h_trkhits_algo = new TH1F("h_trkhits_algo", "", 100,0,100);
  TH1F* h_trkhits_frozen = new TH1F("h_trkhits_frozen", "", 100,0,100);
  TH1F* h_trkhits_usetot = new TH1F("h_trkhits_usetot", "", 100,0,100);
  TH1F* h_trkhits_usedriftdt = new TH1F("h_trkhits_usedriftdt", "", 100,0,100);
  TH1F* h_trkhits_useabsdt = new TH1F("h_trkhits_useabsdt", "", 100,0,100);
  TH1F* h_trkhits_usendvar = new TH1F("h_trkhits_usendvar", "", 100,0,100);
  TH1F* h_trkhits_bkgqual = new TH1F("h_trkhits_bkgqual", "", 100,0,100);
  TH1F* h_trkhits_signqual = new TH1F("h_trkhits_signqual", "", 100,-10,10);
  TH1F* h_trkhits_driftqual = new TH1F("h_trkhits_driftqual", "", 100,-10,10);
  TH1F* h_trkhits_chi2qual = new TH1F("h_trkhits_chi2qual", "", 100,-100,0);
  TH1F* h_trkhits_earlyend = new TH1F("h_trkhits_earlyend", "", 50,0,5);
  TH1F* h_trkhits_edep = new TH1F("h_trkhits_edep", "", 100,0,1);
  TH1F* h_trkhits_wdist = new TH1F("h_trkhits_wdist", "", 100,-200,200);
  TH1F* h_trkhits_werr = new TH1F("h_trkhits_werr", "", 150,0,150);
  TH1F* h_trkhits_tottdrift = new TH1F("h_trkhits_tottdrift", "", 100,0,100);
  TH1F* h_trkhits_etime_0 = new TH1F("h_trkhits_etime_0", "", 200,0,2000);
  TH1F* h_trkhits_etime_1 = new TH1F("h_trkhits_etime_1", "", 200,0,2000);
  TH1F* h_trkhits_tot_0 = new TH1F("h_trkhits_tot_0", "", 100,0,100);
  TH1F* h_trkhits_tot_1 = new TH1F("h_trkhits_tot_1", "", 100,0,100);
  TH1F* h_trkhits_ptoca = new TH1F("h_trkhits_ptoca", "", 200,0,2000);
  TH1F* h_trkhits_stoca = new TH1F("h_trkhits_stoca", "", 200,0,2000);
  TH1F* h_trkhits_rdoca = new TH1F("h_trkhits_rdoca", "", 100,-10,10);
  TH1F* h_trkhits_rdocavar = new TH1F("h_trkhits_rdocavar", "", 100,0,100);
  TH1F* h_trkhits_rdt = new TH1F("h_trkhits_rdt", "", 100,-100,100);
  TH1F* h_trkhits_rtocavar = new TH1F("h_trkhits_rtocavar", "", 100,0,100);
  TH1F* h_trkhits_udoca = new TH1F("h_trkhits_udoca", "", 100,-100,100);
  TH1F* h_trkhits_udocavar = new TH1F("h_trkhits_udocavar", "", 100,-100,100);
  TH1F* h_trkhits_udt = new TH1F("h_trkhits_udt", "", 100,-100,100);
  TH1F* h_trkhits_utocavar = new TH1F("h_trkhits_utocavar", "", 100,-100,100);
  TH1F* h_trkhits_rupos = new TH1F("h_trkhits_rupos", "", 100,-500,500);
  TH1F* h_trkhits_uupos = new TH1F("h_trkhits_uupos", "", 100,-500,500);
  TH1F* h_trkhits_rdrift = new TH1F("h_trkhits_rdrift", "", 100,-10,10);
  TH1F* h_trkhits_cdrift = new TH1F("h_trkhits_cdrift", "", 100,-10,10);
  TH1F* h_trkhits_sderr = new TH1F("h_trkhits_sderr", "", 100,0,100);
  TH1F* h_trkhits_uderr = new TH1F("h_trkhits_uderr", "", 100,0,100);
  TH1F* h_trkhits_dvel = new TH1F("h_trkhits_dvel", "", 100,0,100);
  TH1F* h_trkhits_lang = new TH1F("h_trkhits_lang", "", 100,0,100);
  TH1F* h_trkhits_utresid = new TH1F("h_trkhits_utresid", "", 100,-100,100);
  TH1F* h_trkhits_utresidmvar = new TH1F("h_trkhits_utresidmvar", "", 100,-500,500);
  TH1F* h_trkhits_utresidpvar = new TH1F("h_trkhits_utresidpvar", "", 100,0,10);
  TH1F* h_trkhits_udresid = new TH1F("h_trkhits_udresid", "", 100,-10,10);
  TH1F* h_trkhits_udresidmvar = new TH1F("h_trkhits_udresidmvar", "", 100,-10,10);
  TH1F* h_trkhits_udresidpvar = new TH1F("h_trkhits_udresidpvar", "", 100,0,100);
  TH1F* h_trkhits_rtresid = new TH1F("h_trkhits_rtresid", "", 100,-100,100);
  TH1F* h_trkhits_rtresidmvar = new TH1F("h_trkhits_rtresidmvar", "", 100,-200,200);
  TH1F* h_trkhits_rtresidpvar = new TH1F("h_trkhits_rtresidpvar", "", 100,-10,10);
  TH1F* h_trkhits_rdresid = new TH1F("h_trkhits_rdresid", "", 100,-10,10);
  TH1F* h_trkhits_rdresidmvar = new TH1F("h_trkhits_rdresidmvar", "", 100,-10,10);
  TH1F* h_trkhits_rdresidpvar = new TH1F("h_trkhits_rdresidpvar", "", 100,-10,10);
  TH1F* h_trkhits_wdot = new TH1F("h_trkhits_wdot", "", 100,-10,10);
  TH1F* h_trkhits_poca_x = new TH1F("h_trkhits_poca_x", "", 100,-500,500);
  TH1F* h_trkhits_poca_y = new TH1F("h_trkhits_poca_y", "", 100,-500,500);
  TH1F* h_trkhits_poca_z = new TH1F("h_trkhits_poca_z", "", 100,-500,500);
  TH1F* h_trkhits_uwirephi = new TH1F("h_trkhits_uwirephi", "", 100,-10,10);
  TH1F* h_trkhits_ustrawdist = new TH1F("h_trkhits_ustrawdist", "", 100,0,100);
  TH1F* h_trkhits_ustrawphi = new TH1F("h_trkhits_ustrawphi", "", 100,-10,10);
  TH1F* h_trkhits_dhit = new TH1F("h_trkhits_dhit", "", 100,0,10);
  TH1F* h_trkhits_dactive = new TH1F("h_trkhits_dactive", "", 100,0,10);

  TH1F* h_trkhitsmc_pdg = new TH1F("h_trkhitsmc_pdg", "", 100,-20,20);
  TH1F* h_trkhitsmc_gen = new TH1F("h_trkhitsmc_gen", "", 100,0,100);
  TH1F* h_trkhitsmc_startCode = new TH1F("h_trkhitsmc_startCode", "", 200,0,200);
  TH1F* h_trkhitsmc_ambig = new TH1F("h_trkhitsmc_ambig", "", 100,-10,10);
  TH1F* h_trkhitsmc_earlyend = new TH1F("h_trkhitsmc_earlyend", "", 100,0,100);
  TH1F* h_trkhitsmc_rel_rel = new TH1F("h_trkhitsmc_rel_rel", "", 110,-10,100);
  TH1F* h_trkhitsmc_rel_rem = new TH1F("h_trkhitsmc_rel_rem", "", 110,-10,100);
  TH1F* h_trkhitsmc_t0 = new TH1F("h_trkhitsmc_t0", "", 200,0,2000);
  TH1F* h_trkhitsmc_tdrift = new TH1F("h_trkhitsmc_tdrift", "", 100,0,100);
  TH1F* h_trkhitsmc_rdrift = new TH1F("h_trkhitsmc_rdrift", "", 100,0,100);
  TH1F* h_trkhitsmc_tprop = new TH1F("h_trkhitsmc_tprop", "", 100,0,100);
  TH1F* h_trkhitsmc_dist = new TH1F("h_trkhitsmc_dist", "", 100,0,100);
  TH1F* h_trkhitsmc_doca = new TH1F("h_trkhitsmc_doca", "", 100,-10,10);
  TH1F* h_trkhitsmc_len = new TH1F("h_trkhitsmc_len", "", 100,-200,200);
  TH1F* h_trkhitsmc_edep = new TH1F("h_trkhitsmc_edep", "", 100,0,1);
  TH1F* h_trkhitsmc_mom = new TH1F("h_trkhitsmc_mom", "", 100,0,200);
  TH1F* h_trkhitsmc_twdot = new TH1F("h_trkhitsmc_twdot", "", 100,-10,10);
  TH1F* h_trkhitsmc_tau = new TH1F("h_trkhitsmc_tau", "", 100,-10,10);
  TH1F* h_trkhitsmc_cdist = new TH1F("h_trkhitsmc_cdist", "", 100,0,100);
  TH1F* h_trkhitsmc_phi = new TH1F("h_trkhitsmc_phi", "", 100,-10,10);
  TH1F* h_trkhitsmc_lang = new TH1F("h_trkhitsmc_lang", "", 100,0,100);
  TH1F* h_trkhitsmc_strawdoca = new TH1F("h_trkhitsmc_strawdoca", "", 100,-10,10);
  TH1F* h_trkhitsmc_strawphi = new TH1F("h_trkhitsmc_strawphi", "", 100,-10,10);
  TH1F* h_trkhitsmc_cpos_x = new TH1F("h_trkhitsmc_cpos_x", "", 100,-500,500);
  TH1F* h_trkhitsmc_cpos_y = new TH1F("h_trkhitsmc_cpos_y", "", 100,-500,500);
  TH1F* h_trkhitsmc_cpos_z = new TH1F("h_trkhitsmc_cpos_z", "", 100,-500,500);
  TH1F* h_trkhitsmc_recohit = new TH1F("h_trkhitsmc_recohit", "", 100,0,100);

  TH1F* h_trkmats_active = new TH1F("h_trkmats_active", "", 100,0,100);
  TH1F* h_trkmats_hashit = new TH1F("h_trkmats_hashit", "", 100,0,100);
  TH1F* h_trkmats_activehit = new TH1F("h_trkmats_activehit", "", 100,0,100);
  TH1F* h_trkmats_drifthit = new TH1F("h_trkmats_drifthit", "", 100,0,100);
  TH1F* h_trkmats_plane = new TH1F("h_trkmats_plane", "", 100,0,100);
  TH1F* h_trkmats_panel = new TH1F("h_trkmats_panel", "", 100,0,100);
  TH1F* h_trkmats_layer = new TH1F("h_trkmats_layer", "", 100,0,100);
  TH1F* h_trkmats_straw = new TH1F("h_trkmats_straw", "", 100,0,100);
  TH1F* h_trkmats_pcalc = new TH1F("h_trkmats_pcalc", "", 100,0,100);
  TH1F* h_trkmats_doca = new TH1F("h_trkmats_doca", "", 100,-100,100);
  TH1F* h_trkmats_dp  = new TH1F("h_trkmats_dp", "", 100,-1,1);
  TH1F* h_trkmats_radlen = new TH1F("h_trkmats_radlen", "", 100,0,100);
  TH1F* h_trkmats_dirdot = new TH1F("h_trkmats_dirdot", "", 100,0,100);
  TH1F* h_trkmats_gaspath = new TH1F("h_trkmats_gaspath", "", 100,0,100);
  TH1F* h_trkmats_wallpath = new TH1F("h_trkmats_wallpath", "", 100,0,100);
  TH1F* h_trkmats_wirepath = new TH1F("h_trkmats_wirepath", "", 100,0,100);
  TH1F* h_trkmats_upos = new TH1F("h_trkmats_upos", "", 100,0,100);
  TH1F* h_trkmats_udist = new TH1F("h_trkmats_udist", "", 100,0,100);
  TH1F* h_trkmats_poca_x = new TH1F("h_trkmats_poca_x", "", 200,-200,200);
  TH1F* h_trkmats_poca_y = new TH1F("h_trkmats_poca_y", "", 200,-200,200);
  TH1F* h_trkmats_poca_z = new TH1F("h_trkmats_poca_z", "", 200,-200,200);

  TH1F* h_trkhitcalibs_dDdX_x = new TH1F("h_trkhitcalibs_dDdX_x", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdX_y = new TH1F("h_trkhitcalibs_dDdX_y", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdX_z = new TH1F("h_trkhitcalibs_dDdX_z", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPlane_0 = new TH1F("h_trkhitcalibs_dDdPlane_0", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPlane_1 = new TH1F("h_trkhitcalibs_dDdPlane_1", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPlane_2 = new TH1F("h_trkhitcalibs_dDdPlane_2", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPlane_3 = new TH1F("h_trkhitcalibs_dDdPlane_3", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPlane_4 = new TH1F("h_trkhitcalibs_dDdPlane_4", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPlane_5 = new TH1F("h_trkhitcalibs_dDdPlane_5", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPanel_0 = new TH1F("h_trkhitcalibs_dDdPanel_0", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPanel_1 = new TH1F("h_trkhitcalibs_dDdPanel_1", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPanel_2 = new TH1F("h_trkhitcalibs_dDdPanel_2", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPanel_3 = new TH1F("h_trkhitcalibs_dDdPanel_3", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPanel_4 = new TH1F("h_trkhitcalibs_dDdPanel_4", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdPanel_5 = new TH1F("h_trkhitcalibs_dDdPanel_5", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdP_0 = new TH1F("h_trkhitcalibs_dDdP_0", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdP_1 = new TH1F("h_trkhitcalibs_dDdP_1", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdP_2 = new TH1F("h_trkhitcalibs_dDdP_2", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdP_3 = new TH1F("h_trkhitcalibs_dDdP_3", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdP_4 = new TH1F("h_trkhitcalibs_dDdP_4", "", 100,0,10);
  TH1F* h_trkhitcalibs_dDdP_5 = new TH1F("h_trkhitcalibs_dDdP_5", "", 100,0,10);
  TH1F* h_trkhitcalibs_dLdP_0 = new TH1F("h_trkhitcalibs_dLdP_0", "", 100,0,10);
  TH1F* h_trkhitcalibs_dLdP_1 = new TH1F("h_trkhitcalibs_dLdP_1", "", 100,0,10);
  TH1F* h_trkhitcalibs_dLdP_2 = new TH1F("h_trkhitcalibs_dLdP_2", "", 100,0,10);
  TH1F* h_trkhitcalibs_dLdP_3 = new TH1F("h_trkhitcalibs_dLdP_3", "", 100,0,10);
  TH1F* h_trkhitcalibs_dLdP_4 = new TH1F("h_trkhitcalibs_dLdP_4", "", 100,0,10);
  TH1F* h_trkhitcalibs_dLdP_5 = new TH1F("h_trkhitcalibs_dLdP_5", "", 100,0,10);


  TH1F* h_hitcount_nsd = new TH1F("h_hitcount_nsd", "", 150,0,150);
  TH1F* h_hitcount_nesel = new TH1F("h_hitcount_nesel", "", 150,0,150);
  TH1F* h_hitcount_nrsel = new TH1F("h_hitcount_nrsel", "", 150,0,150);
  TH1F* h_hitcount_ntsel = new TH1F("h_hitcount_ntsel", "", 150,0,150);
  TH1F* h_hitcount_nbkg = new TH1F("h_hitcount_nbkg", "", 100,0,100);

  TH1F* h_crvsummary_totalPEs = new TH1F("h_crvsummary_totalPEs", "", 200,0,200);
  TH1F* h_crvsummary_nHitCounters = new TH1F("h_crvsummary_nHitCounters", "", 25,0,25);

  TH1F* h_crvsummarymc_totalEnergyDeposited = new TH1F("h_crvsummarymc_totalEnergyDeposited", "", 100,0,100);
  TH1F* h_crvsummarymc_minPathLayer = new TH1F("h_crvsummarymc_minPathLayer", "", 100,0,100);
  TH1F* h_crvsummarymc_maxPathLayer = new TH1F("h_crvsummarymc_maxPathLayer", "", 100,0,100);
  TH1F* h_crvsummarymc_nHitCounters = new TH1F("h_crvsummarymc_nHitCounters", "", 100,0,100);
  TH1F* h_crvsummarymc_pos_x = new TH1F("h_crvsummarymc_pos_x", "", 100,-500,500);
  TH1F* h_crvsummarymc_pos_y = new TH1F("h_crvsummarymc_pos_y", "", 100,-500,500);
  TH1F* h_crvsummarymc_pos_z = new TH1F("h_crvsummarymc_pos_z", "", 100,-500,500);
  TH1F* h_crvsummarymc_sectorNumber = new TH1F("h_crvsummarymc_sectorNumber", "", 100,-25,25);
  TH1F* h_crvsummarymc_sectorType = new TH1F("h_crvsummarymc_sectorType", "", 100,-10,10);
  TH1F* h_crvsummarymc_pdgId = new TH1F("h_crvsummarymc_pdgId", "", 40,-20,20);

  TH1F* h_crvdigis_adc = new TH1F("h_crvdigis_adc", "", 100,0,100);
  TH1F* h_crvdigis_time = new TH1F("h_crvdigis_time", "", 100,0,100);
  TH1F* h_crvdigis_SiPMId = new TH1F("h_crvdigis_SiPMId", "", 100,0,100);

  TH1F* h_crvpulses_pos_x = new TH1F("h_crvpulses_pos_x", "", 100,0,100);
  TH1F* h_crvpulses_pos_y = new TH1F("h_crvpulses_pos_y", "", 100,0,100);
  TH1F* h_crvpulses_pos_z = new TH1F("h_crvpulses_pos_z", "", 100,0,100);
  TH1F* h_crvpulses_barId = new TH1F("h_crvpulses_barId", "", 100,0,100);
  TH1F* h_crvpulses_sectorId = new TH1F("h_crvpulses_sectorId", "", 100,0,100);
  TH1F* h_crvpulses_SiPMId = new TH1F("h_crvpulses_SiPMId", "", 100,0,100);
  TH1F* h_crvpulses_PEs = new TH1F("h_crvpulses_PEs", "", 100,0,100);
  TH1F* h_crvpulses_PEsPulseHeight = new TH1F("h_crvpulses_PEsPulseHeight", "", 100,0,100);
  TH1F* h_crvpulses_pulseHeight = new TH1F("h_crvpulses_pulseHeight", "", 100,0,100);
  TH1F* h_crvpulses_pulseBeta = new TH1F("h_crvpulses_pulseBeta", "", 100,0,100);
  TH1F* h_crvpulses_pulseFitChi2 = new TH1F("h_crvpulses_pulseFitChi2", "", 100,0,100);
  TH1F* h_crvpulses_time = new TH1F("h_crvpulses_time", "", 100,0,100);

  TH1F* h_crvpulsesmc_valid = new TH1F("h_crvpulsesmc_valid", "", 100,0,100);
  TH1F* h_crvpulsesmc_pdgId = new TH1F("h_crvpulsesmc_pdgId", "", 100,0,100);
  TH1F* h_crvpulsesmc_primaryPdgId = new TH1F("h_crvpulsesmc_primaryPdgId", "", 100,0,100);
  TH1F* h_crvpulsesmc_primaryE = new TH1F("h_crvpulsesmc_primaryE", "", 100,0,100);
  TH1F* h_crvpulsesmc_primary_x = new TH1F("h_crvpulsesmc_primary_x", "", 100,0,100);
  TH1F* h_crvpulsesmc_primary_y = new TH1F("h_crvpulsesmc_primary_y", "", 100,0,100);
  TH1F* h_crvpulsesmc_primary_z = new TH1F("h_crvpulsesmc_primary_z", "", 100,0,100);
  TH1F* h_crvpulsesmc_parentPdgId = new TH1F("h_crvpulsesmc_parentPdgId", "", 100,0,100);
  TH1F* h_crvpulsesmc_parentE = new TH1F("h_crvpulsesmc_parentE", "", 100,0,100);
  TH1F* h_crvpulsesmc_parent_x = new TH1F("h_crvpulsesmc_parent_x", "", 100,0,100);
  TH1F* h_crvpulsesmc_parent_y = new TH1F("h_crvpulsesmc_parent_y", "", 100,0,100);
  TH1F* h_crvpulsesmc_parent_z = new TH1F("h_crvpulsesmc_parent_z", "", 100,0,100);
  TH1F* h_crvpulsesmc_gparentPdgId = new TH1F("h_crvpulsesmc_gparentPdgId", "", 100,0,100);
  TH1F* h_crvpulsesmc_gparentE = new TH1F("h_crvpulsesmc_gparentE", "", 100,0,100);
  TH1F* h_crvpulsesmc_gparent_x = new TH1F("h_crvpulsesmc_gparent_x", "", 100,0,100);
  TH1F* h_crvpulsesmc_gparent_y = new TH1F("h_crvpulsesmc_gparent_y", "", 100,0,100);
  TH1F* h_crvpulsesmc_gparent_z = new TH1F("h_crvpulsesmc_gparent_z", "", 100,0,100);
  TH1F* h_crvpulsesmc_pos_x = new TH1F("h_crvpulsesmc_pos_x", "", 100,0,100);
  TH1F* h_crvpulsesmc_pos_y = new TH1F("h_crvpulsesmc_pos_y", "", 100,0,100);
  TH1F* h_crvpulsesmc_pos_z = new TH1F("h_crvpulsesmc_pos_z", "", 100,0,100);
  TH1F* h_crvpulsesmc_time = new TH1F("h_crvpulsesmc_time", "", 100,0,100);
  TH1F* h_crvpulsesmc_depositedEnergy = new TH1F("h_crvpulsesmc_depositedEnergy", "", 100,0,100);

  TH1F* h_crvcoicsmcplane_pdgId = new TH1F("h_crvcoincsmcplane_pdgId", "", 40,-20,20);
  TH1F* h_crvcoicsmcplane_primaryPdgId = new TH1F("h_crvcoincsmcplane_primaryPdgId", "", 40,-20,20);
  TH1F* h_crvcoicsmcplane_primaryE = new TH1F("h_crvcoincsmcplane_primaryE", "", 100,0,5000);
  TH1F* h_crvcoicsmcplane_primary_x = new TH1F("h_crvcoincsmcplane_primary_x", "", 100,-500,500);
  TH1F* h_crvcoicsmcplane_primary_y = new TH1F("h_crvcoincsmcplane_primary_y", "", 100,-500,500);
  TH1F* h_crvcoicsmcplane_primary_z = new TH1F("h_crvcoincsmcplane_primary_z", "", 100,-500,500);
  TH1F* h_crvcoicsmcplane_pos_x = new TH1F("h_crvcoincsmcplane_pos_x", "", 100,-500,500);
  TH1F* h_crvcoicsmcplane_pos_y = new TH1F("h_crvcoincsmcplane_pos_y", "", 100,-500,500);
  TH1F* h_crvcoicsmcplane_pos_z = new TH1F("h_crvcoincsmcplane_pos_z", "", 100,-500,500);
  TH1F* h_crvcoicsmcplane_dir_x = new TH1F("h_crvcoincsmcplane_dir_x", "", 100,-1,1);
  TH1F* h_crvcoicsmcplane_dir_y = new TH1F("h_crvcoincsmcplane_dir_y", "", 100,-1,1);
  TH1F* h_crvcoicsmcplane_dir_z = new TH1F("h_crvcoincsmcplane_dir_z", "", 100,-1,1);
  TH1F* h_crvcoicsmcplane_time = new TH1F("h_crvcoincsmcplane_time", "", 200,0,2000);
  TH1F* h_crvcoicsmcplane_kineticEnergy = new TH1F("h_crvcoincsmcplane_kineticEnergy", "", 200,0,200);
  TH1F* h_crvcoicsmcplane_dataSource = new TH1F("h_crvcoincsmcplane_dataSource", "", 100,0,100);

  TH1F* h_caloclusters_diskID_ = new TH1F("h_caloclusters_diskID_", "", 2,0,2);
  TH1F* h_caloclusters_time_ = new TH1F("h_caloclusters_time_", "", 200,0,2000);
  TH1F* h_caloclusters_timeErr_ = new TH1F("h_caloclusters_timeErr_", "", 100,0,100);
  TH1F* h_caloclusters_energyDep_ = new TH1F("h_caloclusters_energyDep_", "", 100,0,100);
  TH1F* h_caloclusters_energyDepErr_ = new TH1F("h_caloclusters_energyDepErr_", "", 100,0,100);
  TH1F* h_caloclusters_cog_x = new TH1F("h_caloclusters_cog_x", "", 100,-500,500);
  TH1F* h_caloclusters_cog_y = new TH1F("h_caloclusters_cog_y", "", 100,-500,500);
  TH1F* h_caloclusters_cog_z = new TH1F("h_caloclusters_cog_z", "", 100,-500,500);
  TH1F* h_caloclusters_hits_ = new TH1F("h_caloclusters_hits_", "", 100,0,100);
  TH1F* h_caloclusters_size_ = new TH1F("h_caloclusters_size_", "", 100,0,100);
  TH1F* h_caloclusters_isSplit_ = new TH1F("h_caloclusters_isSplit_", "", 2,0,2);

  TH1F* h_calohits_crystalId_ = new TH1F("h_calohits_crystalId_", "", 2,0,2);
  TH1F* h_calohits_nSiPMs_ = new TH1F("h_calohits_nSiPMs_", "", 100,0,100);
  TH1F* h_calohits_time_ = new TH1F("h_calohits_time_", "", 200,0,2000);
  TH1F* h_calohits_timeErr_ = new TH1F("h_calohits_timeErr_", "", 100,0,100);
  TH1F* h_calohits_eDep_ = new TH1F("h_calohits_eDep_", "", 100,0,100);
  TH1F* h_calohits_eDepErr_ = new TH1F("h_calohits_eDepErr_", "", 100,0,100);
  TH1F* h_calohits_recoDigis_ = new TH1F("h_calohits_recoDigis_", "", 100,0,100);
  TH1F* h_calohits_clusterIdx_ = new TH1F("h_calohits_clusterIdx_", "", 100,0,100);

  TH1F* h_calorecodigis_eDep_ = new TH1F("h_calorecodigis_eDep_", "", 100,0,100);
  TH1F* h_calorecodigis_eDepErr_ = new TH1F("h_calorecodigis_eDepErr_", "", 100,0,100);
  TH1F* h_calorecodigis_time_ = new TH1F("h_calorecodigis_time_", "", 200,0,2000);
  TH1F* h_calorecodigis_timeErr_ = new TH1F("h_calorecodigis_timeErr_", "", 100,0,100);
  TH1F* h_calorecodigis_chi2_ = new TH1F("h_calorecodigis_chi2_", "", 100,0,100);
  TH1F* h_calorecodigis_ndf_ = new TH1F("h_calorecodigis_ndf_", "", 100,0,100);
  TH1F* h_calorecodigis_pileUp_ = new TH1F("h_calorecodigis_pileUp_", "", 2,0,2);
  TH1F* h_calorecodigis_caloDigiIdx_ = new TH1F("h_calorecodigis_caloDigiIdx_", "", 100,0,100);
  TH1F* h_calorecodigis_caloHitIdx_ = new TH1F("h_calorecodigis_caloHitIdx_", "", 100,0,100);

  TH1F* h_calodigis_SiPMID_ = new TH1F("h_calodigis_SiPMID_", "", 100,0,100);
  TH1F* h_calodigis_t0_ = new TH1F("h_calodigis_t0_", "", 100,0,100);
  TH1F* h_calodigis_waveform_ = new TH1F("h_calodigis_waveform_", "", 100,0,100);
  TH1F* h_calodigis_peakpos_ = new TH1F("h_calodigis_peakpos_", "", 100,0,100);
  TH1F* h_calodigis_caloRecoDigiIdx_ = new TH1F("h_calodigis_caloRecoDigiIdx_", "", 100,0,100);

  TH1F* h_triginfo = new TH1F("h_triginfo", "", mu2e::TrigInfo::ntrig_,0,mu2e::TrigInfo::ntrig_);
  // Grab the first event to name the bins
  const auto& evt = util.GetEvent(0);
  for (const auto pair : evt.trigNameMap) {
    h_triginfo->GetXaxis()->SetBinLabel(pair.second+1, pair.first.c_str());
  }

  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    std::cout << "Event #" << i_event << std::endl;
    const auto& event = util.GetEvent(i_event);

    std::cout << "Creating evtinfo histograms..." << std::endl;
    h_evtinfo_event->Fill(event.evtinfo->event);
    h_evtinfo_subrun->Fill(event.evtinfo->subrun);
    h_evtinfo_run->Fill(event.evtinfo->run);
    h_evtinfo_nprotons->Fill(event.evtinfo->nprotons);
    h_evtinfo_pbtime->Fill(event.evtinfo->pbtime);
    h_evtinfo_pbterr->Fill(event.evtinfo->pbterr);

    h_evtinfomc_nprotons->Fill(event.evtinfomc->nprotons);
    h_evtinfomc_pbtime->Fill(event.evtinfomc->pbtime);

    std::cout << "Creating trk histograms..." << std::endl;
    for (const auto& trk : *(event.trk)) {
      h_trk_status->Fill(trk.status);
      h_trk_goodfit->Fill(trk.goodfit);
      h_trk_seedalg->Fill(trk.seedalg);
      h_trk_fitalg->Fill(trk.fitalg);
      h_trk_pdg->Fill(trk.pdg);
      h_trk_nhits->Fill(trk.nhits);
      h_trk_ndof->Fill(trk.ndof);
      h_trk_nactive->Fill(trk.nactive);
      h_trk_ndouble->Fill(trk.ndouble);
      h_trk_ndactive->Fill(trk.ndactive);
      h_trk_nplanes->Fill(trk.nplanes);
      h_trk_planespan->Fill(trk.planespan);
      h_trk_nnullambig->Fill(trk.nnullambig);
      h_trk_nmat->Fill(trk.nmat);
      h_trk_nmatactive->Fill(trk.nmatactive);
      h_trk_nesel->Fill(trk.nesel);
      h_trk_nrsel->Fill(trk.nrsel);
      h_trk_ntsel->Fill(trk.ntsel);
      h_trk_nbkg->Fill(trk.nbkg);
      h_trk_nsel->Fill(trk.nsel);
      h_trk_nseg->Fill(trk.nseg);
      h_trk_nipaup->Fill(trk.nipaup);
      h_trk_nipadown->Fill(trk.nipadown);
      h_trk_nstup->Fill(trk.nstup);
      h_trk_nstdown->Fill(trk.nstdown);
      h_trk_tsdainter->Fill(trk.tsdainter);
      h_trk_opainter->Fill(trk.opainter);
      h_trk_firststinter->Fill(trk.firststinter);
      h_trk_chisq->Fill(trk.chisq);
      h_trk_fitcon->Fill(trk.fitcon);
      h_trk_radlen->Fill(trk.radlen);
      h_trk_firsthit->Fill(trk.firsthit);
      h_trk_lasthit->Fill(trk.lasthit);
      h_trk_maxgap->Fill(trk.maxgap);
      h_trk_avggap->Fill(trk.avggap);
      h_trk_avgedep->Fill(trk.avgedep);
    }

    std::cout << "Creating trkseg histograms..." << std::endl;
    for (const auto& trksegs : *(event.trksegs)) {
      for (const auto& trkseg : trksegs) {
        h_trksegs_mom_x->Fill(trkseg.mom.x());
        h_trksegs_mom_y->Fill(trkseg.mom.y());
        h_trksegs_mom_z->Fill(trkseg.mom.z());
        h_trksegs_mom_mag->Fill(trkseg.mom.R());
        h_trksegs_pos_x->Fill(trkseg.pos.x());
        h_trksegs_pos_y->Fill(trkseg.pos.y());
        h_trksegs_pos_z->Fill(trkseg.pos.z());
        h_trksegs_time->Fill(trkseg.time);
        h_trksegs_dmom->Fill(trkseg.dmom);
        h_trksegs_momerr->Fill(trkseg.momerr);
        h_trksegs_inbounds->Fill(trkseg.inbounds);
        h_trksegs_gap->Fill(trkseg.gap);
        h_trksegs_sid->Fill(trkseg.sid);
        h_trksegs_sindex->Fill(trkseg.sindex);
      }
    }

    std::cout << "Creating trkmc histograms..." << std::endl;
    for (const auto& trkmc : *(event.trkmc)) {
      h_trkmc_valid->Fill(trkmc.valid);
      h_trkmc_ndigi->Fill(trkmc.ndigi);
      h_trkmc_ndigigood->Fill(trkmc.ndigigood);
      h_trkmc_nhits->Fill(trkmc.nhits);
      h_trkmc_nactive->Fill(trkmc.nactive);
      h_trkmc_nambig->Fill(trkmc.nambig);
      h_trkmc_nipaup->Fill(trkmc.nipaup);
      h_trkmc_nipadown->Fill(trkmc.nipadown);
      h_trkmc_nstup->Fill(trkmc.nstup);
      h_trkmc_nstdown->Fill(trkmc.nstdown);
      h_trkmc_maxr->Fill(trkmc.maxr);
      h_trkmc_rad->Fill(trkmc.rad);
      h_trkmc_lam->Fill(trkmc.lam);
      h_trkmc_cx->Fill(trkmc.cx);
      h_trkmc_cy->Fill(trkmc.cy);
      h_trkmc_phi0->Fill(trkmc.phi0);
      h_trkmc_t0->Fill(trkmc.t0);
    }

    std::cout << "Creating trksegsmc histograms..." << std::endl;
    for (const auto& trksegsmc : *(event.trksegsmc)) {
      for (const auto& trksegmc : trksegsmc) {
        h_trksegsmc_sid->Fill(trksegmc.sid);
        h_trksegsmc_sindex->Fill(trksegmc.sindex);
        h_trksegsmc_time->Fill(trksegmc.time);
        h_trksegsmc_edep->Fill(trksegmc.edep);
        h_trksegsmc_path->Fill(trksegmc.path);
        h_trksegsmc_pos_x->Fill(trksegmc.pos.x());
        h_trksegsmc_pos_y->Fill(trksegmc.pos.y());
        h_trksegsmc_pos_z->Fill(trksegmc.pos.z());
        h_trksegsmc_mom_x->Fill(trksegmc.mom.x());
        h_trksegsmc_mom_y->Fill(trksegmc.mom.y());
        h_trksegsmc_mom_z->Fill(trksegmc.mom.z());
      }
    }

    std::cout << "Creating trkcalohit histograms..." << std::endl;
    for (const auto& trkcalohit : *(event.trkcalohit)) {
      h_trkcalohit_active->Fill(trkcalohit.active);
      h_trkcalohit_did->Fill(trkcalohit.did);
      h_trkcalohit_poca_x->Fill(trkcalohit.poca.x());
      h_trkcalohit_poca_y->Fill(trkcalohit.poca.y());
      h_trkcalohit_poca_z->Fill(trkcalohit.poca.z());
      h_trkcalohit_mom_x->Fill(trkcalohit.mom.x());
      h_trkcalohit_mom_y->Fill(trkcalohit.mom.y());
      h_trkcalohit_mom_z->Fill(trkcalohit.mom.z());
      h_trkcalohit_cdepth->Fill(trkcalohit.cdepth);
      h_trkcalohit_trkdepth->Fill(trkcalohit.trkdepth);
      h_trkcalohit_dphidot->Fill(trkcalohit.dphidot);
      h_trkcalohit_doca->Fill(trkcalohit.doca);
      h_trkcalohit_dt->Fill(trkcalohit.dt);
      h_trkcalohit_ptoca->Fill(trkcalohit.ptoca);
      h_trkcalohit_tocavar->Fill(trkcalohit.tocavar);
      h_trkcalohit_tresid->Fill(trkcalohit.tresid);
      h_trkcalohit_tresidmvar->Fill(trkcalohit.tresidmvar);
      h_trkcalohit_tresidpvar->Fill(trkcalohit.tresidpvar);
      h_trkcalohit_ctime->Fill(trkcalohit.ctime);
      h_trkcalohit_ctimeerr->Fill(trkcalohit.ctimeerr);
      h_trkcalohit_csize->Fill(trkcalohit.csize);
      h_trkcalohit_edep->Fill(trkcalohit.edep);
      h_trkcalohit_edeperr->Fill(trkcalohit.edeperr);
    }

    std::cout << "Creating trkcalohitmc histograms..." << std::endl;
    for (const auto& trkcalohitmc : *(event.trkcalohitmc)) {
      h_trkcalohitmc_nsim->Fill(trkcalohitmc.nsim);
      h_trkcalohitmc_etot->Fill(trkcalohitmc.etot);
      h_trkcalohitmc_tavg->Fill(trkcalohitmc.tavg);
      h_trkcalohitmc_eprimary->Fill(trkcalohitmc.eprimary);
      h_trkcalohitmc_tprimary->Fill(trkcalohitmc.tprimary);
      h_trkcalohitmc_prel->Fill(trkcalohitmc.prel.relationship());
    }

    std::cout << "Creating crvcoinc histograms..." << std::endl;
    for (const auto& crvcoinc : *(event.crvcoincs)) {
      h_crvcoincs_sectorType->Fill(crvcoinc.sectorType);
      h_crvcoincs_pos_x->Fill(crvcoinc.pos.x());
      h_crvcoincs_pos_y->Fill(crvcoinc.pos.y());
      h_crvcoincs_pos_z->Fill(crvcoinc.pos.z());
      h_crvcoincs_timeStart->Fill(crvcoinc.timeStart);
      h_crvcoincs_timeEnd->Fill(crvcoinc.timeEnd);
      h_crvcoincs_time->Fill(crvcoinc.time);
      h_crvcoincs_PEs->Fill(crvcoinc.PEs);
      for (const auto& layer_PEs : crvcoinc.PEsPerLayer) {
        h_crvcoincs_PEsPerLayer->Fill(layer_PEs);
      }
      h_crvcoincs_nHits->Fill(crvcoinc.nHits);
      h_crvcoincs_nLayers->Fill(crvcoinc.nLayers);
      h_crvcoincs_angle->Fill(crvcoinc.angle);
    }

    std::cout << "Creating crvcoincmc histograms..." << std::endl;
    for (const auto& crvcoincmc : *(event.crvcoincsmc)) {
      h_crvcoincsmc_valid->Fill(crvcoincmc.valid);
      h_crvcoincsmc_pdgId->Fill(crvcoincmc.pdgId);
      h_crvcoincsmc_primaryPdgId->Fill(crvcoincmc.primaryPdgId);
      h_crvcoincsmc_primaryE->Fill(crvcoincmc.primaryE);
      h_crvcoincsmc_primary_x->Fill(crvcoincmc.primary.x());
      h_crvcoincsmc_primary_y->Fill(crvcoincmc.primary.y());
      h_crvcoincsmc_primary_z->Fill(crvcoincmc.primary.z());
      h_crvcoincsmc_parentPdgId->Fill(crvcoincmc.parentPdgId);
      h_crvcoincsmc_parentE->Fill(crvcoincmc.parentE);
      h_crvcoincsmc_parent_x->Fill(crvcoincmc.parent.x());
      h_crvcoincsmc_parent_y->Fill(crvcoincmc.parent.y());
      h_crvcoincsmc_parent_z->Fill(crvcoincmc.parent.z());
      h_crvcoincsmc_gparentPdgId->Fill(crvcoincmc.gparentPdgId);
      h_crvcoincsmc_gparentE->Fill(crvcoincmc.gparentE);
      h_crvcoincsmc_gparent_x->Fill(crvcoincmc.gparent.x());
      h_crvcoincsmc_gparent_y->Fill(crvcoincmc.gparent.y());
      h_crvcoincsmc_gparent_z->Fill(crvcoincmc.gparent.z());
      h_crvcoincsmc_pos_x->Fill(crvcoincmc.pos.x());
      h_crvcoincsmc_pos_y->Fill(crvcoincmc.pos.y());
      h_crvcoincsmc_pos_z->Fill(crvcoincmc.pos.z());
      h_crvcoincsmc_time->Fill(crvcoincmc.time);
      h_crvcoincsmc_depositedEnergy->Fill(crvcoincmc.depositedEnergy);
    }

    std::cout << "Creating trkmcsim histograms..." << std::endl;
    for (const auto& trkmcsims : *(event.trkmcsim)) {
      for (const auto& trkmcsim : trkmcsims) {
        h_trkmcsim_valid->Fill(trkmcsim.valid);
        h_trkmcsim_id->Fill(trkmcsim.id);
        h_trkmcsim_nhits->Fill(trkmcsim.nhits);
        h_trkmcsim_nactive->Fill(trkmcsim.nactive);
        h_trkmcsim_rank->Fill(trkmcsim.rank);
        h_trkmcsim_pdg->Fill(trkmcsim.pdg);
        h_trkmcsim_startCode->Fill(trkmcsim.startCode);
        h_trkmcsim_stopCode->Fill(trkmcsim.stopCode);
        h_trkmcsim_gen->Fill(trkmcsim.gen);
        h_trkmcsim_time->Fill(trkmcsim.time);
        h_trkmcsim_index->Fill(trkmcsim.index);
        h_trkmcsim_mom_x->Fill(trkmcsim.mom.x());
        h_trkmcsim_mom_y->Fill(trkmcsim.mom.y());
        h_trkmcsim_mom_z->Fill(trkmcsim.mom.z());
        h_trkmcsim_pos_x->Fill(trkmcsim.pos.x());
        h_trkmcsim_pos_y->Fill(trkmcsim.pos.y());
        h_trkmcsim_pos_z->Fill(trkmcsim.pos.z());
        h_trkmcsim_endmom_x->Fill(trkmcsim.endmom.x());
        h_trkmcsim_endmom_y->Fill(trkmcsim.endmom.y());
        h_trkmcsim_endmom_z->Fill(trkmcsim.endmom.z());
        h_trkmcsim_endpos_x->Fill(trkmcsim.endpos.x());
        h_trkmcsim_endpos_y->Fill(trkmcsim.endpos.y());
        h_trkmcsim_endpos_z->Fill(trkmcsim.endpos.z());
        h_trkmcsim_prirel_rel->Fill(trkmcsim.prirel.relationship());
        h_trkmcsim_prirel_rem->Fill(trkmcsim.prirel.removal());
        h_trkmcsim_trkrel_rel->Fill(trkmcsim.trkrel.relationship());
        h_trkmcsim_trkrel_rem->Fill(trkmcsim.trkrel.removal());
      }
    }

    std::cout << "Creating trkqual histograms..." << std::endl;
    for (const auto& trkqual : *(event.trkqual)) {
      h_trkqual_valid->Fill(trkqual.valid);
      h_trkqual_result->Fill(trkqual.result);
    }

    if (event.trksegpars_lh != nullptr) { // might not have this branch
      std::cout << "Creating trksegpars_lh histograms..." << std::endl;
      for (const auto& track : *(event.trksegpars_lh)) {
        for (const auto& trksegpars_lh : track) {
          h_trksegpars_lh_maxr->Fill(trksegpars_lh.maxr);
          h_trksegpars_lh_d0->Fill(trksegpars_lh.d0);
          h_trksegpars_lh_tanDip->Fill(trksegpars_lh.tanDip);
          h_trksegpars_lh_rad->Fill(trksegpars_lh.rad);
          h_trksegpars_lh_lam->Fill(trksegpars_lh.lam);
          h_trksegpars_lh_cx->Fill(trksegpars_lh.cx);
          h_trksegpars_lh_cy->Fill(trksegpars_lh.cy);
          h_trksegpars_lh_phi0->Fill(trksegpars_lh.phi0);
          h_trksegpars_lh_t0->Fill(trksegpars_lh.t0);
          h_trksegpars_lh_raderr->Fill(trksegpars_lh.raderr);
          h_trksegpars_lh_lamerr->Fill(trksegpars_lh.lamerr);
          h_trksegpars_lh_cxerr->Fill(trksegpars_lh.cxerr);
          h_trksegpars_lh_cyerr->Fill(trksegpars_lh.cyerr);
          h_trksegpars_lh_phi0err->Fill(trksegpars_lh.phi0err);
          h_trksegpars_lh_t0err->Fill(trksegpars_lh.t0err);
        }
      }
    }

    if (event.trksegpars_ch != nullptr) { // might not have this branch
      std::cout << "Creating trksegpars_ch histograms..." << std::endl;
      for (const auto& track : *(event.trksegpars_ch)) {
        for (const auto& trksegpars_ch : track) {
          h_trksegpars_ch_d0->Fill(trksegpars_ch.d0);
          h_trksegpars_ch_phi0->Fill(trksegpars_ch.phi0);
          h_trksegpars_ch_omega->Fill(trksegpars_ch.omega);
          h_trksegpars_ch_z0->Fill(trksegpars_ch.z0);
          h_trksegpars_ch_tanDip->Fill(trksegpars_ch.tanDip);
          h_trksegpars_ch_t0->Fill(trksegpars_ch.t0);
          h_trksegpars_ch_d0err->Fill(trksegpars_ch.d0err);
          h_trksegpars_ch_phi0err->Fill(trksegpars_ch.phi0err);
          h_trksegpars_ch_omegaerr->Fill(trksegpars_ch.omegaerr);
          h_trksegpars_ch_z0err->Fill(trksegpars_ch.z0err);
          h_trksegpars_ch_tanDiperr->Fill(trksegpars_ch.tanDiperr);
          h_trksegpars_ch_t0err->Fill(trksegpars_ch.t0err);
          h_trksegpars_ch_maxr->Fill(trksegpars_ch.maxr);
        }
      }
    }

    if (event.trksegpars_kl != nullptr) { // might not have this branch
      std::cout << "Creating trksegpars_kl histograms..." << std::endl;
      for (const auto& track : *(event.trksegpars_kl)) {
        for (const auto& trksegpars_kl : track) {
          h_trksegpars_kl_d0->Fill(trksegpars_kl.d0);
          h_trksegpars_kl_phi0->Fill(trksegpars_kl.phi0);
          h_trksegpars_kl_z0->Fill(trksegpars_kl.z0);
          h_trksegpars_kl_theta->Fill(trksegpars_kl.theta);
          h_trksegpars_kl_mom->Fill(trksegpars_kl.mom);
          h_trksegpars_kl_t0->Fill(trksegpars_kl.t0);
          h_trksegpars_kl_d0err->Fill(trksegpars_kl.d0err);
          h_trksegpars_kl_phi0err->Fill(trksegpars_kl.phi0err);
          h_trksegpars_kl_z0err->Fill(trksegpars_kl.z0err);
          h_trksegpars_kl_thetaerr->Fill(trksegpars_kl.thetaerr);
          h_trksegpars_kl_momerr->Fill(trksegpars_kl.momerr);
          h_trksegpars_kl_t0err->Fill(trksegpars_kl.t0err);
        }
      }
    }

    if (event.trkhits != nullptr) { // might not have this branch
      std::cout << "Creating trkhits histograms..." << std::endl;
      for (const auto& trkhits : *(event.trkhits)) {
        for (const auto& trkhit : trkhits) {
          h_trkhits_plane->Fill(trkhit.plane);
          h_trkhits_panel->Fill(trkhit.panel);
          h_trkhits_layer->Fill(trkhit.layer);
          h_trkhits_straw->Fill(trkhit.straw);
          h_trkhits_state->Fill(trkhit.state);
          h_trkhits_algo->Fill(trkhit.algo);
          h_trkhits_frozen->Fill(trkhit.frozen);
          h_trkhits_usetot->Fill(trkhit.usetot);
          h_trkhits_usedriftdt->Fill(trkhit.usedriftdt);
          h_trkhits_useabsdt->Fill(trkhit.useabsdt);
          h_trkhits_usendvar->Fill(trkhit.usendvar);
          h_trkhits_bkgqual->Fill(trkhit.bkgqual);
          h_trkhits_signqual->Fill(trkhit.signqual);
          h_trkhits_driftqual->Fill(trkhit.driftqual);
          h_trkhits_chi2qual->Fill(trkhit.chi2qual);
          h_trkhits_earlyend->Fill(trkhit.earlyend);
          h_trkhits_edep->Fill(trkhit.edep);
          h_trkhits_wdist->Fill(trkhit.wdist);
          h_trkhits_werr->Fill(trkhit.werr);
          h_trkhits_tottdrift->Fill(trkhit.tottdrift);
          h_trkhits_etime_0->Fill(trkhit.etime[0]);
          h_trkhits_etime_1->Fill(trkhit.etime[1]);
          h_trkhits_tot_0->Fill(trkhit.tot[0]);
          h_trkhits_tot_1->Fill(trkhit.tot[1]);
          h_trkhits_ptoca->Fill(trkhit.ptoca);
          h_trkhits_stoca->Fill(trkhit.stoca);
          h_trkhits_rdoca->Fill(trkhit.rdoca);
          h_trkhits_rdocavar->Fill(trkhit.rdocavar);
          h_trkhits_rdt->Fill(trkhit.rdt);
          h_trkhits_rtocavar->Fill(trkhit.rtocavar);
          h_trkhits_udoca->Fill(trkhit.udoca);
          h_trkhits_udocavar->Fill(trkhit.udocavar);
          h_trkhits_udt->Fill(trkhit.udt);
          h_trkhits_utocavar->Fill(trkhit.utocavar);
          h_trkhits_rupos->Fill(trkhit.rupos);
          h_trkhits_uupos->Fill(trkhit.uupos);
          h_trkhits_rdrift->Fill(trkhit.rdrift);
          h_trkhits_cdrift->Fill(trkhit.cdrift);
          h_trkhits_sderr->Fill(trkhit.sderr);
          h_trkhits_uderr->Fill(trkhit.uderr);
          h_trkhits_dvel->Fill(trkhit.dvel);
          h_trkhits_lang->Fill(trkhit.lang);
          h_trkhits_utresid->Fill(trkhit.utresid);
          h_trkhits_utresidmvar->Fill(trkhit.utresidmvar);
          h_trkhits_utresidpvar->Fill(trkhit.utresidpvar);
          h_trkhits_udresid->Fill(trkhit.udresid);
          h_trkhits_udresidmvar->Fill(trkhit.udresidmvar);
          h_trkhits_udresidpvar->Fill(trkhit.udresidpvar);
          h_trkhits_rtresid->Fill(trkhit.rtresid);
          h_trkhits_rtresidmvar->Fill(trkhit.rtresidmvar);
          h_trkhits_rtresidpvar->Fill(trkhit.rtresidpvar);
          h_trkhits_rdresid->Fill(trkhit.rdresid);
          h_trkhits_rdresidmvar->Fill(trkhit.rdresidmvar);
          h_trkhits_rdresidpvar->Fill(trkhit.rdresidpvar);
          h_trkhits_wdot->Fill(trkhit.wdot);
          h_trkhits_poca_x->Fill(trkhit.poca.x());
          h_trkhits_poca_y->Fill(trkhit.poca.y());
          h_trkhits_poca_z->Fill(trkhit.poca.z());
          h_trkhits_uwirephi->Fill(trkhit.uwirephi);
          h_trkhits_ustrawdist->Fill(trkhit.ustrawdist);
          h_trkhits_ustrawphi->Fill(trkhit.ustrawphi);
          h_trkhits_dhit->Fill(trkhit.dhit);
          h_trkhits_dactive->Fill(trkhit.dactive);
        }
      }
    }

    if (event.trkhitsmc != nullptr) { // might not have this branch
      std::cout << "Creating trkhitsmc histograms..." << std::endl;
      for (const auto& trkhitsmc : *(event.trkhitsmc)) {
        for (const auto& trkhitmc : trkhitsmc) {
          h_trkhitsmc_pdg->Fill(trkhitmc.pdg);
          h_trkhitsmc_gen->Fill(trkhitmc.gen);
          h_trkhitsmc_startCode->Fill(trkhitmc.startCode);
          h_trkhitsmc_ambig->Fill(trkhitmc.ambig);
          h_trkhitsmc_earlyend->Fill(trkhitmc.earlyend);
          h_trkhitsmc_rel_rel->Fill(trkhitmc.rel.relationship());
          h_trkhitsmc_rel_rem->Fill(trkhitmc.rel.removal());
          h_trkhitsmc_t0->Fill(trkhitmc.t0);
          h_trkhitsmc_tdrift->Fill(trkhitmc.tdrift);
          h_trkhitsmc_rdrift->Fill(trkhitmc.rdrift);
          h_trkhitsmc_tprop->Fill(trkhitmc.tprop);
          h_trkhitsmc_dist->Fill(trkhitmc.dist);
          h_trkhitsmc_doca->Fill(trkhitmc.doca);
          h_trkhitsmc_len->Fill(trkhitmc.len);
          h_trkhitsmc_edep->Fill(trkhitmc.edep);
          h_trkhitsmc_mom->Fill(trkhitmc.mom);
          h_trkhitsmc_twdot->Fill(trkhitmc.twdot);
          h_trkhitsmc_tau->Fill(trkhitmc.tau);
          h_trkhitsmc_cdist->Fill(trkhitmc.cdist);
          h_trkhitsmc_phi->Fill(trkhitmc.phi);
          h_trkhitsmc_lang->Fill(trkhitmc.lang);
          h_trkhitsmc_strawdoca->Fill(trkhitmc.strawdoca);
          h_trkhitsmc_strawphi->Fill(trkhitmc.strawphi);
          h_trkhitsmc_cpos_x->Fill(trkhitmc.cpos.x());
          h_trkhitsmc_cpos_y->Fill(trkhitmc.cpos.y());
          h_trkhitsmc_cpos_z->Fill(trkhitmc.cpos.z());
          h_trkhitsmc_recohit->Fill(trkhitmc.recohit);
        }
      }
    }

    if (event.trkmats != nullptr) { // might not have this branch
      std::cout << "Creating trkmats histograms..." << std::endl;
      for (const auto& trkmats : *(event.trkmats)) {
        for (const auto& trkmat : trkmats) {
          h_trkmats_active->Fill(trkmat.active);
          h_trkmats_hashit->Fill(trkmat.hashit);
          h_trkmats_activehit->Fill(trkmat.activehit);
          h_trkmats_drifthit->Fill(trkmat.drifthit);
          h_trkmats_plane->Fill(trkmat.plane);
          h_trkmats_panel->Fill(trkmat.panel);
          h_trkmats_layer->Fill(trkmat.layer);
          h_trkmats_straw->Fill(trkmat.straw);
          h_trkmats_pcalc->Fill(trkmat.pcalc);
          h_trkmats_doca->Fill(trkmat.doca);
          h_trkmats_dp->Fill(trkmat.dp);
          h_trkmats_radlen->Fill(trkmat.radlen);
          h_trkmats_dirdot->Fill(trkmat.dirdot);
          h_trkmats_gaspath->Fill(trkmat.gaspath);
          h_trkmats_wallpath->Fill(trkmat.wallpath);
          h_trkmats_wirepath->Fill(trkmat.wirepath);
          h_trkmats_upos->Fill(trkmat.upos);
          h_trkmats_udist->Fill(trkmat.udist);
          h_trkmats_poca_x->Fill(trkmat.poca.x());
          h_trkmats_poca_y->Fill(trkmat.poca.y());
          h_trkmats_poca_z->Fill(trkmat.poca.z());
        }
      }
    }

    if (event.trkhitcalibs != nullptr) { // might not have this branch
      std::cout << "Creating trkhitcalibs histograms..." << std::endl;
      for (const auto& trkhitcalibs : *(event.trkhitcalibs)) {
        for (const auto& trkhitcalib : trkhitcalibs) {
          h_trkhitcalibs_dDdX_x->Fill(trkhitcalib.dDdX.x());
          h_trkhitcalibs_dDdX_y->Fill(trkhitcalib.dDdX.y());
          h_trkhitcalibs_dDdX_z->Fill(trkhitcalib.dDdX.z());
          h_trkhitcalibs_dDdPlane_0->Fill(trkhitcalib.dDdPlane[0]);
          h_trkhitcalibs_dDdPlane_1->Fill(trkhitcalib.dDdPlane[1]);
          h_trkhitcalibs_dDdPlane_2->Fill(trkhitcalib.dDdPlane[2]);
          h_trkhitcalibs_dDdPlane_3->Fill(trkhitcalib.dDdPlane[3]);
          h_trkhitcalibs_dDdPlane_4->Fill(trkhitcalib.dDdPlane[4]);
          h_trkhitcalibs_dDdPlane_5->Fill(trkhitcalib.dDdPlane[5]);
          h_trkhitcalibs_dDdPanel_0->Fill(trkhitcalib.dDdPanel[0]);
          h_trkhitcalibs_dDdPanel_1->Fill(trkhitcalib.dDdPanel[1]);
          h_trkhitcalibs_dDdPanel_2->Fill(trkhitcalib.dDdPanel[2]);
          h_trkhitcalibs_dDdPanel_3->Fill(trkhitcalib.dDdPanel[3]);
          h_trkhitcalibs_dDdPanel_4->Fill(trkhitcalib.dDdPanel[4]);
          h_trkhitcalibs_dDdPanel_5->Fill(trkhitcalib.dDdPanel[5]);
          h_trkhitcalibs_dDdP_0->Fill(trkhitcalib.dDdP[0]);
          h_trkhitcalibs_dDdP_1->Fill(trkhitcalib.dDdP[1]);
          h_trkhitcalibs_dDdP_2->Fill(trkhitcalib.dDdP[2]);
          h_trkhitcalibs_dDdP_3->Fill(trkhitcalib.dDdP[3]);
          h_trkhitcalibs_dDdP_4->Fill(trkhitcalib.dDdP[4]);
          h_trkhitcalibs_dDdP_5->Fill(trkhitcalib.dDdP[5]);
          h_trkhitcalibs_dLdP_0->Fill(trkhitcalib.dLdP[0]);
          h_trkhitcalibs_dLdP_1->Fill(trkhitcalib.dLdP[1]);
          h_trkhitcalibs_dLdP_2->Fill(trkhitcalib.dLdP[2]);
          h_trkhitcalibs_dLdP_3->Fill(trkhitcalib.dLdP[3]);
          h_trkhitcalibs_dLdP_4->Fill(trkhitcalib.dLdP[4]);
          h_trkhitcalibs_dLdP_5->Fill(trkhitcalib.dLdP[5]);
        }
      }
    }

    std::cout << "Creating hitcount histograms..." << std::endl;
    auto hitcount = *(event.hitcount);
    h_hitcount_nsd->Fill(hitcount.nsd);
    h_hitcount_nesel->Fill(hitcount.nesel);
    h_hitcount_nrsel->Fill(hitcount.nrsel);
    h_hitcount_ntsel->Fill(hitcount.ntsel);
    h_hitcount_nbkg->Fill(hitcount.nbkg);

    std::cout << "Creating crvsummary histograms..." << std::endl;
    auto crvsummary = *(event.crvsummary);
    h_crvsummary_totalPEs->Fill(crvsummary.totalPEs);
    h_crvsummary_nHitCounters->Fill(crvsummary.nHitCounters);

    std::cout << "Creating crvsummarymc histograms..." << std::endl;
    auto crvsummarymc = *(event.crvsummarymc);
    h_crvsummarymc_totalEnergyDeposited->Fill(crvsummarymc.totalEnergyDeposited);
    h_crvsummarymc_minPathLayer->Fill(crvsummarymc.minPathLayer);
    h_crvsummarymc_maxPathLayer->Fill(crvsummarymc.maxPathLayer);
    h_crvsummarymc_nHitCounters->Fill(crvsummarymc.nHitCounters);
    h_crvsummarymc_pos_x->Fill(crvsummarymc.pos.x());
    h_crvsummarymc_pos_y->Fill(crvsummarymc.pos.y());
    h_crvsummarymc_pos_z->Fill(crvsummarymc.pos.z());
    h_crvsummarymc_sectorNumber->Fill(crvsummarymc.sectorNumber);
    h_crvsummarymc_sectorType->Fill(crvsummarymc.sectorType);
    h_crvsummarymc_pdgId->Fill(crvsummarymc.pdgId);

    if (event.crvdigis != nullptr) {
      std::cout << "Creating crvdigi histograms..." << std::endl;
      for (const auto& crvdigi : *(event.crvdigis)) {
        h_crvdigis_adc->Fill(crvdigi.adc);
        h_crvdigis_time->Fill(crvdigi.time);
        h_crvdigis_SiPMId->Fill(crvdigi.SiPMId);
      }
    }

    if (event.crvpulses != nullptr) {
      std::cout << "Creating crvpulse histograms..." << std::endl;
      for (const auto& crvpulse : *(event.crvpulses)) {
        h_crvpulses_pos_x->Fill(crvpulse.pos.x());
        h_crvpulses_pos_y->Fill(crvpulse.pos.y());
        h_crvpulses_pos_z->Fill(crvpulse.pos.z());
        h_crvpulses_barId->Fill(crvpulse.barId);
        h_crvpulses_sectorId->Fill(crvpulse.sectorId);
        h_crvpulses_SiPMId->Fill(crvpulse.SiPMId);
        h_crvpulses_PEs->Fill(crvpulse.PEs);
        h_crvpulses_PEsPulseHeight->Fill(crvpulse.PEsPulseHeight);
        h_crvpulses_pulseHeight->Fill(crvpulse.pulseHeight);
        h_crvpulses_pulseBeta->Fill(crvpulse.pulseBeta);
        h_crvpulses_pulseFitChi2->Fill(crvpulse.pulseFitChi2);
        h_crvpulses_time->Fill(crvpulse.time);
      }
    }


    if (event.crvpulsesmc != nullptr) {
      std::cout << "Creating crvpulsesmc histograms..." << std::endl;
      for (const auto& crvpulsemc : *(event.crvpulsesmc)) {
        h_crvpulsesmc_valid->Fill(crvpulsemc.valid);
        h_crvpulsesmc_pdgId->Fill(crvpulsemc.pdgId);
        h_crvpulsesmc_primaryPdgId->Fill(crvpulsemc.primaryPdgId);
        h_crvpulsesmc_primaryE->Fill(crvpulsemc.primaryE);
        h_crvpulsesmc_primary_x->Fill(crvpulsemc.primary.x());
        h_crvpulsesmc_primary_y->Fill(crvpulsemc.primary.y());
        h_crvpulsesmc_primary_z->Fill(crvpulsemc.primary.z());
        h_crvpulsesmc_parentPdgId->Fill(crvpulsemc.parentPdgId);
        h_crvpulsesmc_parentE->Fill(crvpulsemc.parentE);
        h_crvpulsesmc_parent_x->Fill(crvpulsemc.parent.x());
        h_crvpulsesmc_parent_y->Fill(crvpulsemc.parent.y());
        h_crvpulsesmc_parent_z->Fill(crvpulsemc.parent.z());
        h_crvpulsesmc_gparentPdgId->Fill(crvpulsemc.gparentPdgId);
        h_crvpulsesmc_gparentE->Fill(crvpulsemc.gparentE);
        h_crvpulsesmc_gparent_x->Fill(crvpulsemc.gparent.x());
        h_crvpulsesmc_gparent_y->Fill(crvpulsemc.gparent.y());
        h_crvpulsesmc_gparent_z->Fill(crvpulsemc.gparent.z());
        h_crvpulsesmc_pos_x->Fill(crvpulsemc.pos.x());
        h_crvpulsesmc_pos_y->Fill(crvpulsemc.pos.y());
        h_crvpulsesmc_pos_z->Fill(crvpulsemc.pos.z());
        h_crvpulsesmc_time->Fill(crvpulsemc.time);
        h_crvpulsesmc_depositedEnergy->Fill(crvpulsemc.depositedEnergy);
      }
    }

    if (event.crvcoincsmcplane != nullptr) {
      std::cout << "Creating crvcoincsmcplane histograms..." << std::endl;
      for (const auto& crvcoincmcplane : *(event.crvcoincsmcplane)) {
        h_crvcoicsmcplane_pdgId->Fill(crvcoincmcplane.pdgId);
        h_crvcoicsmcplane_primaryPdgId->Fill(crvcoincmcplane.primaryPdgId);
        h_crvcoicsmcplane_primaryE->Fill(crvcoincmcplane.primaryE);
        h_crvcoicsmcplane_primary_x->Fill(crvcoincmcplane.primary.x());
        h_crvcoicsmcplane_primary_y->Fill(crvcoincmcplane.primary.y());
        h_crvcoicsmcplane_primary_z->Fill(crvcoincmcplane.primary.z());
        h_crvcoicsmcplane_pos_x->Fill(crvcoincmcplane.pos.x());
        h_crvcoicsmcplane_pos_y->Fill(crvcoincmcplane.pos.y());
        h_crvcoicsmcplane_pos_z->Fill(crvcoincmcplane.pos.z());
        h_crvcoicsmcplane_dir_x->Fill(crvcoincmcplane.dir.x());
        h_crvcoicsmcplane_dir_y->Fill(crvcoincmcplane.dir.y());
        h_crvcoicsmcplane_dir_z->Fill(crvcoincmcplane.dir.z());
        h_crvcoicsmcplane_time->Fill(crvcoincmcplane.time);
        h_crvcoicsmcplane_kineticEnergy->Fill(crvcoincmcplane.kineticEnergy);
        h_crvcoicsmcplane_dataSource->Fill(crvcoincmcplane.dataSource);
      }
    }


    if (event.caloclusters != nullptr) {
      std::cout << "Creating caloclusters histograms..." << std::endl;
      for (const auto& calocluster : *(event.caloclusters)) {
        h_caloclusters_diskID_->Fill(calocluster.diskID_);
        h_caloclusters_time_->Fill(calocluster.time_);
        h_caloclusters_timeErr_->Fill(calocluster.timeErr_);
        h_caloclusters_energyDep_->Fill(calocluster.energyDep_);
        h_caloclusters_energyDepErr_->Fill(calocluster.energyDepErr_);
        h_caloclusters_cog_x->Fill(calocluster.cog_.x());
        h_caloclusters_cog_y->Fill(calocluster.cog_.y());
        h_caloclusters_cog_z->Fill(calocluster.cog_.z());
        for (const auto& i_hit : calocluster.hits_) {
          h_caloclusters_hits_->Fill(i_hit);
        }
        h_caloclusters_size_->Fill(calocluster.size_);
        h_caloclusters_isSplit_->Fill(calocluster.isSplit_);
      }
    }

    if (event.calohits != nullptr) {
      std::cout << "Creating calohits histograms..." << std::endl;
      for (const auto& calohit : *(event.calohits)) {

        h_calohits_crystalId_->Fill(calohit.crystalId_);
        h_calohits_nSiPMs_->Fill(calohit.nSiPMs_);
        h_calohits_time_->Fill(calohit.time_);
        h_calohits_timeErr_->Fill(calohit.timeErr_);
        h_calohits_eDep_->Fill(calohit.eDep_);
        h_calohits_eDepErr_->Fill(calohit.eDepErr_);
        for (const auto& i_reco_digi : calohit.recoDigis_) {
          h_calohits_recoDigis_->Fill(i_reco_digi);
        }
        h_calohits_clusterIdx_->Fill(calohit.clusterIdx_);
      }
    }

    if (event.calorecodigis != nullptr) {
      std::cout << "Creating calorecodigis histograms..." << std::endl;
      for (const auto& calorecodigi : *(event.calorecodigis)) {
        h_calorecodigis_eDep_->Fill(calorecodigi.eDep_);
        h_calorecodigis_eDepErr_->Fill(calorecodigi.eDepErr_);
        h_calorecodigis_time_->Fill(calorecodigi.time_);
        h_calorecodigis_timeErr_->Fill(calorecodigi.timeErr_);
        h_calorecodigis_chi2_->Fill(calorecodigi.chi2_);
        h_calorecodigis_ndf_->Fill(calorecodigi.ndf_);
        h_calorecodigis_pileUp_->Fill(calorecodigi.pileUp_);
        h_calorecodigis_caloDigiIdx_->Fill(calorecodigi.caloDigiIdx_);
        h_calorecodigis_caloHitIdx_->Fill(calorecodigi.caloHitIdx_);
      }
    }

    if (event.calodigis != nullptr) {
      std::cout << "Creating calodigis histograms..." << std::endl;
      for (const auto& calodigi : *(event.calodigis)) {

        h_calodigis_SiPMID_->Fill(calodigi.SiPMID_);
        h_calodigis_t0_->Fill(calodigi.t0_);
        for (const auto& i_waveform : calodigi.waveform_) {
          h_calodigis_waveform_->Fill(i_waveform);
        }
        h_calodigis_peakpos_->Fill(calodigi.peakpos_);
        h_calodigis_caloRecoDigiIdx_->Fill(calodigi.caloRecoDigiIdx_);
      }
    }

    // Creating triginfo histogram
    std::cout << "Creating triginfo histogram" << std::endl;
    for (int i_trig = 0; i_trig < mu2e::TrigInfo::ntrig_; ++i_trig) {
      h_triginfo->AddBinContent(i_trig+1, event.triginfo._triggerArray[i_trig]);
    }

  }

  file->Write();
  file->Close();
}
