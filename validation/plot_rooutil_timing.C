void plot_rooutil_timing() {

  TTree* tree = new TTree();
  tree->ReadFile("test_rooutil_timing.log", "", ',');
  //  tree->Print();
  TCanvas* c2 = new TCanvas("c2", "c2", 800,600);
  c2->Divide(2,2);
  c2->cd(1);
  tree->Draw("time_per_event_ns*1e-3>>h_time_per_event_us");
  c2->cd(2);
  tree->Draw("event_loop_time_ns*1e-6>>h_event_loop_time_ms");
  c2->cd(3);
  tree->Draw("setup_time_ns*1e-6>>h_setup_time_ms");
  c2->cd(4);
  tree->Draw("total_time_ns*1e-6>>h_total_time_ms");
}
