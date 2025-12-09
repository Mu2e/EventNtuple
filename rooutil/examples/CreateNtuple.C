//
// An example of how to create a flatter tree if you want to do an unbinned lieklihood fit
// This uses cut functions defined in common_cuts.hh
//

#include "EventNtuple/rooutil/inc/RooUtil.hh"
#include "EventNtuple/rooutil/inc/common_cuts.hh"

#include "TH1F.h"

using namespace rooutil;
void CreateNtuple(std::string filename) {

  // Set up RooUtil
  RooUtil util(filename);

  // Create an output file
  TFile* outfile = new TFile("example_ntuple.root", "RECREATE");
  util.CreateOutputEventNtuple(outfile);

  // Loop through the events
  for (int i_event = 0; i_event < util.GetNEvents(); ++i_event) {
    auto& event = util.GetEvent(i_event);

    event.SelectTracks(is_e_minus);
    if (event.CountTracks() == 1) {
      util.FillOutputEventNtuple();
    }
  }

  outfile->Write();
  outfile->Close();
}
