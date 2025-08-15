//
// TrkInfo: information about the reconstructed track
// All momenta are in units of MeV/c, time in nsec WRT when the proton bunch pulse peak hits the production target,
// positions are in mm WRT the center of the tracker.
// Dave Brown (LBNL)
//
#ifndef TrkInfo_HH
#define TrkInfo_HH

namespace mu2e
{
  struct TrkInfo {
    int status = -1; // Kalman fit status
    int goodfit = 0; // true if fit is geometrically consistent
    int seedalg = -1; // pattern recognition algorithm used
    int fitalg = -1; // final fit algorithm used
    int pdg = 0;   // PDG code of particle assumed in fit
    int nhits = 0;     // number of hits associated to this track
    int ndof = 0;      // number of degrees of freedom in the fit
    int nactive = 0;   // number of hits actually used in the fit ("active")
    int ndouble = 0; // number of double-hit panels
    int ndactive = 0; // number of active double-hit panels
    int nplanes = 0; // number of planes hit
    int npanels = 0;
    int planespan = 0; // number of planes between first and last plane
    int nnullambig = 0;  // number of hits without any left/right ambiguity assigned
    int nmat = 0; // number of materials (straws) assigned to the fit
    int nmatactive = 0; // number materials (straw) assigned to and used in the fit
    int nesel = 0; // number of active hits passing energy selection
    int nrsel = 0; // number of active hits passing radius selection
    int ntsel = 0; // number of active hits passing time selection
    int nbkg = 0; // number of active hits passing background selection
    int nsel = 0; // number of active hits passing all selections

    int nseg =0;     // number of trajectory segments
    int nipaup = 0; // number of upstream IPA intersections
    int nipadown = 0; // number of downstream IPA intersections
    int nstup = 0; // number of upstream stopping target foil intersections
    int nstdown = 0; // number of downstream stopping target foil intersections
    bool tsdainter = false; // does track intersect the TSDA?
    bool opainter = false; // does track intersect the OPA?
    int firststinter=-1; // first intersection with a stopping target foil
    float chisq =-1;   // Kalman fit chisquared
    float fitcon =-1;  // Kalman fit chisqured consistency
    float radlen;  // total radiation length of (active) material crossed by this particle inside the tracker
    float firsthit = 0; // first hit (time or distance)
    float lasthit = 0;  // last hit (time or distance)
    float maxgap = -1; // maximum gap between active hits in fit trajectory [mm]
    float avggap = -1; // average gap between active hits in fit trajectory [mm]
    float avgedep = 0; // average energy deposit of active hits [MeV/c]

    void reset() { *this = TrkInfo(); }
  };
}
#endif
