void show_val_plots(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");

  TH1F* h_trksegs_mom_mag = (TH1F*) file->Get("h_trksegs_mom_mag");
  TH1F* h_crvcoincs_PEs = (TH1F*) file->Get("h_crvcoincs_PEs");
  TH1F* h_caloclusters_energyDep_ = (TH1F*) file->Get("h_caloclusters_energyDep_");
  TH1F* h_mcsteps_virtualdetector_pdg = (TH1F*) file->Get("h_mcsteps_virtualdetector_pdg");

  TCanvas* c1 = new TCanvas();
  c1->Divide(2,2);
  c1->cd(1);
  if (h_trksegs_mom_mag) {
    h_trksegs_mom_mag->Draw();
  }
  c1->cd(2);
  if (h_crvcoincs_PEs) {
    h_crvcoincs_PEs->Draw();
  }
  c1->cd(3);
  if (h_caloclusters_energyDep_) {
    h_caloclusters_energyDep_->Draw();
  }
  c1->cd(4);
  if (h_mcsteps_virtualdetector_pdg) {
    h_mcsteps_virtualdetector_pdg->Draw();
  }

}
