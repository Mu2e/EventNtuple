//
// art module to create the EventNtuple
//

// Mu2e includes
#include "Offline/GeneralUtilities/inc/ParameterSetHelpers.hh"
#include "Offline/MCDataProducts/inc/ProtonBunchIntensity.hh"
#include "Offline/MCDataProducts/inc/EventWeight.hh"
#include "Offline/MCDataProducts/inc/KalSeedMC.hh"
#include "Offline/MCDataProducts/inc/CaloClusterMC.hh"
#include "Offline/MCDataProducts/inc/CaloHitMC.hh"
#include "Offline/MCDataProducts/inc/ProtonBunchTimeMC.hh"
#include "Offline/RecoDataProducts/inc/KalSeed.hh"
#include "Offline/RecoDataProducts/inc/KalSeedAssns.hh"
#include "Offline/RecoDataProducts/inc/CaloCluster.hh"
#include "Offline/RecoDataProducts/inc/CaloHit.hh"
#include "Offline/RecoDataProducts/inc/CaloRecoDigi.hh"
#include "Offline/RecoDataProducts/inc/TrkCaloHitPID.hh"
#include "Offline/RecoDataProducts/inc/ProtonBunchTime.hh"
#include "Offline/TrkReco/inc/TrkUtilities.hh"
#include "Offline/CalorimeterGeom/inc/DiskCalorimeter.hh"
#include "Offline/GeometryService/inc/VirtualDetector.hh"
#include "Offline/RecoDataProducts/inc/CrvCoincidenceCluster.hh"
#include "Offline/MCDataProducts/inc/CrvCoincidenceClusterMC.hh"
#include "Offline/MCDataProducts/inc/CrvCoincidenceClusterMCAssns.hh"
#include "Offline/MCDataProducts/inc/CrvDigiMC.hh"
#include "Offline/Mu2eUtilities/inc/fromStrings.hh"
#include "Offline/TrackerGeom/inc/Tracker.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"
#include "Offline/DataProducts/inc/SurfaceId.hh"
// Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "canvas/Persistency/Common/TriggerResults.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/OptionalSequence.h"

// ROOT incldues
#include "Rtypes.h"
#include "TBits.h"
#include "TTree.h"
#include "TProfile.h"

#include "Offline/Mu2eUtilities/inc/TriggerResultsNavigator.hh"
// diagnostics
#include "EventNtuple/inc/HitCount.hh"
#include "EventNtuple/inc/TrkCount.hh"
#include "EventNtuple/inc/EventInfo.hh"
#include "EventNtuple/inc/EventInfoMC.hh"
#include "EventNtuple/inc/TrkInfo.hh"
#include "EventNtuple/inc/TrkInfoMC.hh"
#include "EventNtuple/inc/TrkSegInfo.hh"
#include "EventNtuple/inc/LoopHelixInfo.hh"
#include "EventNtuple/inc/CentralHelixInfo.hh"
#include "EventNtuple/inc/KinematicLineInfo.hh"
#include "EventNtuple/inc/SimInfo.hh"
#include "EventNtuple/inc/EventWeightInfo.hh"
#include "EventNtuple/inc/TrkStrawHitInfo.hh"
#include "EventNtuple/inc/TrkStrawHitCalibInfo.hh"
#include "EventNtuple/inc/TrkStrawHitInfoMC.hh"
#include "EventNtuple/inc/TrkCaloHitInfo.hh"
#include "EventNtuple/inc/CaloClusterInfoMC.hh"
#include "EventNtuple/inc/CaloHitInfoMC.hh"
#include "EventNtuple/inc/HelixInfo.hh"
#include "EventNtuple/inc/TimeClusterInfo.hh"
#include "EventNtuple/inc/InfoStructHelper.hh"
#include "EventNtuple/inc/CrvInfoHelper.hh"
#include "EventNtuple/inc/TrigInfo.hh"
#include "EventNtuple/inc/InfoMCStructHelper.hh"
#include "Offline/RecoDataProducts/inc/RecoQual.hh"
#include "Offline/RecoDataProducts/inc/MVAResult.hh"
#include "EventNtuple/inc/RecoQualInfo.hh"
#include "EventNtuple/inc/MVAResultInfo.hh"
#include "EventNtuple/inc/BestCrvAssns.hh"
#include "EventNtuple/inc/MCStepInfo.hh"
#include "EventNtuple/inc/SurfaceStepInfo.hh"
#include "EventNtuple/inc/MCStepSummaryInfo.hh"
#include "EventNtuple/inc/CaloDigiMCInfo.hh"
#include "Offline/MCDataProducts/inc/CaloShowerSim.hh"

// C++ includes.
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

namespace mu2e {
  // Need this for the BaBar headers.
  using CLHEP::Hep3Vector;
  typedef KalSeedCollection::const_iterator KSCIter;
  typedef size_t TrkFitBranchIndex;
  typedef size_t StepCollIndex;

  class EventNtupleMaker : public art::EDAnalyzer {

    public:

      // ── Per-branch options (defaults match intended use; omit options in FCL
      //    unless you want to override a specific field) ──────────────────────
      struct TrkFitOptConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        fhicl::Atom<bool> fillmc{Name("fillMC"), Comment("Fill MC information for this branch")};
        fhicl::Atom<bool> fillhits{Name("fillHits"), Comment("Fill hit-level information for this branch")};
        fhicl::Atom<int> genealogyDepth{Name("genealogyDepth"), Comment("Depth of MC genealogy to keep (-1 = all)")};
        fhicl::Atom<int> matchDepth{Name("matchDepth"), Comment("Depth of MC truth matching to keep (-1 = all)")};
      };

      struct TrkFitConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        fhicl::Atom<std::string> input{Name("input"), Comment("KalSeedCollection input tag")};
        fhicl::Atom<std::string> branchname{Name("branchname"), Comment("Name of output branch")};
        fhicl::Atom<bool> fill{Name("fill"), Comment("Set false to skip this branch entirely (no collection reads, no output branches)")};
        fhicl::Sequence<std::string> trkQualTags{Name("trkQualTags"), Comment("Input tags for MVAResultCollection to use for TrkQuals")};
        fhicl::Sequence<std::string> trkPIDTags{Name("trkPIDTags"), Comment("Input tags for MVAResultCollection to use for TrkPID")};
        fhicl::Table<TrkFitOptConfig> options{Name("options"), Comment("Per-branch fill options")};
      };

      // ── Event-level MC config (tracker+CRV share these event products) ─────
      struct EventMCConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        fhicl::Atom<bool> fill{Name("fill"),
          Comment("Master gate for tracker MC, Calo MC, CRV MC, and event-level MC info branches.")};
        fhicl::Atom<art::InputTag> PBTMCTag{Name("PBTMCTag"), Comment("Tag for ProtonBunchTimeMC object")};
        fhicl::Atom<art::InputTag> simParticlesTag{Name("simParticlesTag"), Comment("SimParticle Collection Tag")};
        fhicl::Atom<art::InputTag> mcTrajectoriesTag{Name("mcTrajectoriesTag"), Comment("MCTrajectory Collection Tag")};
        fhicl::Atom<art::InputTag> primaryParticleTag{Name("primaryParticleTag"), Comment("Tag for PrimaryParticle")};
        fhicl::Table<InfoMCStructHelper::Config> infoMCStructHelper{Name("infoMCStructHelper"), Comment("Configuration for InfoMCStructHelper")};
      };

      // ── Tracker subsystem config ───────────────────────────────────────────
      struct TrkConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        // master switch
        fhicl::Atom<bool> fill{Name("fill"), Comment("Enable the tracker subsystem entirely")};
        // reco flags
        fhicl::Atom<bool> fillHits{Name("fillHits"),
          Comment("Global enable for hit-level branches; per-branch options.fillHits also required")};
        fhicl::Atom<bool> fillHitCalibs{Name("fillHitCalibs"), Comment("Fill hit calibration branches")};
        fhicl::Atom<bool> fillTrkQual{Name("fillTrkQual"), Comment("Fill TrkQual MVA branches")};
        fhicl::Atom<bool> fillTrkPID{Name("fillTrkPID"), Comment("Fill TrkPID MVA branches")};
        // per-branch configurations
        fhicl::Sequence<fhicl::Table<TrkFitConfig>> fits{Name("fits"), Comment("KalSeed collections to write into a single track branch")};
        // tracker MC sub-config
        struct MCConfig {
          using Name=fhicl::Name;
          using Comment=fhicl::Comment;
          fhicl::Atom<bool> fill{Name("fill"), Comment("Master switch for all tracker MC branches; mc.fill must also be true")};
          fhicl::Atom<art::InputTag> kalSeedMCAssns{Name("kalSeedMCAssns"), Comment("Tag for KalSeedMCAssn")};
        };
        fhicl::Table<MCConfig> mc{Name("mc"), Comment("Tracker MC filling options")};
      };

      // ── Helix seed config (independent of per-branch track config) ─────────
      struct HelixConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        fhicl::Atom<bool> fill{Name("fill"), Comment("Fill helix seed branches")};
      };

      // ── Time cluster config (independent of per-branch track config) ───────
      struct TimeClusterConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        fhicl::Atom<bool>          fill{Name("fill"), Comment("Fill time cluster branch")};
        fhicl::Atom<art::InputTag> tag {Name("tag"),  Comment("Tag for time cluster collection")};
      };

      // ── MC step collections config (independent of per-branch track config) ─
      struct MCStepsConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        fhicl::OptionalSequence<art::InputTag> extraMCStepTags{Name("extraMCStepTags"), Comment("Tags for extra StepPointMCCollections associated with KalSeeds")};
        fhicl::OptionalAtom<art::InputTag>     surfaceStepsTag{Name("surfaceStepsTag"), Comment("SurfaceStep collection tag; omit to disable surface step branches")};
        fhicl::OptionalSequence<art::InputTag> stepPointMCTags{Name("stepPointMCTags"), Comment("Tags for global StepPointMCCollections (not per-track)")};
      };

      // ── Calorimeter subsystem config ───────────────────────────────────────
      // Calo MC is gated solely by calo.mc.fill; it is independent of mc.fill
      // so that calo-only jobs can fill calo MC without loading tracker MC products.
      struct CaloConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        // master switch
        fhicl::Atom<bool> fill{Name("fill"), Comment("Enable the calorimeter subsystem entirely")};
        // reco flags + collocated tags
        fhicl::Atom<bool>          fillClusters {Name("fillClusters"),  Comment("Fill calorimeter cluster branch")};
        fhicl::Atom<art::InputTag> clustersTag  {Name("clustersTag"),   Comment("Tag for CaloCluster collection")};
        fhicl::Atom<bool>          fillHits     {Name("fillHits"),      Comment("Fill calorimeter hit branch")};
        fhicl::Atom<art::InputTag> hitsTag      {Name("hitsTag"),       Comment("Tag for CaloHit collection")};
        fhicl::Atom<bool>          fillRecoDigis{Name("fillRecoDigis"), Comment("Fill calorimeter reco-digi branch")};
        fhicl::Atom<art::InputTag> recoDigisTag {Name("recoDigisTag"),  Comment("Tag for CaloRecoDigi collection")};
        fhicl::Atom<bool>          fillDigis    {Name("fillDigis"),     Comment("Fill calorimeter digi branch")};
        fhicl::Atom<art::InputTag> digisTag     {Name("digisTag"),      Comment("Tag for CaloDigi collection")};
        // MC sub-config
        struct MCConfig {
          using Name=fhicl::Name;
          using Comment=fhicl::Comment;
          fhicl::Atom<bool> fill{Name("fill"), Comment("Master switch for all calorimeter MC branches; independent of mc.fill")};
          // track-associated calo MC (the <branch>calohitmc. branch per track type)
          fhicl::Atom<bool> fillTrackMatch{Name("fillTrackMatch"), Comment("Fill per-track calo cluster MC branch (<branch>calohitmc.)")};
          // standalone calo MC branches
          fhicl::Atom<bool>          fillClusters{Name("fillClusters"), Comment("Fill standalone caloclustersmc. branch")};
          fhicl::Atom<art::InputTag> clusterMCTag{Name("clusterMCTag"), Comment("Tag for CaloClusterMCCollection")};
          fhicl::Atom<bool>          fillHits    {Name("fillHits"),     Comment("Fill standalone calohitsmc. branch")};
          fhicl::Atom<art::InputTag> hitMCTag    {Name("hitMCTag"),     Comment("Tag for CaloHitMCCollection")};
          fhicl::Atom<bool>          fillSim     {Name("fillSim"),      Comment("Fill calomcsim. (sim particle info) branch")};
          fhicl::Atom<bool>          fillDigis   {Name("fillDigis"),    Comment("Fill standalone calodigismc. branch")};
          fhicl::Atom<bool>          fillDigiSim {Name("fillDigiSim"),  Comment("Fill calodigisim. branch")};
          fhicl::Atom<art::InputTag> showerSimTag{Name("showerSimTag"), Comment("Tag for CaloShowerSim collection")};
        };
        fhicl::Table<MCConfig> mc{Name("mc"), Comment("Calorimeter MC filling options")};
      };

      // ── CRV subsystem config ───────────────────────────────────────────────
      struct CRVConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        // master switch
        fhicl::Atom<bool> fill{Name("fill"), Comment("Enable the CRV subsystem entirely")};
        // reco flags + collocated tags
        fhicl::Atom<bool>          fillCoincs    {Name("fillCoincs"),      Comment("Fill CRV coincidence cluster branches")};
        fhicl::Atom<art::InputTag> coincidencesTag{Name("coincidencesTag"), Comment("Tag for CrvCoincidenceCluster collection")};
        fhicl::Atom<art::InputTag> recoPulsesTag  {Name("recoPulsesTag"),   Comment("Tag for CrvRecoPulse collection")};
        fhicl::Atom<art::InputTag> stepsTag       {Name("stepsTag"),        Comment("Tag for CrvStep collection")};
        fhicl::Atom<bool>          fillPulses     {Name("fillPulses"),      Comment("Fill CRV reco pulse branches")};
        fhicl::Atom<bool>          fillDigis      {Name("fillDigis"),       Comment("Fill CRV digi branch")};
        fhicl::Atom<art::InputTag> digisTag       {Name("digisTag"),        Comment("Tag for CrvDigi collection")};
        fhicl::Atom<double>        planeY         {Name("planeY"),          Comment("Y of center of top layer of CRV-T counters (mm)")};
        fhicl::OptionalAtom<art::InputTag> inferenceTag{Name("inferenceTag"), Comment("Tag for CrvInference Assns (art::Assns<KalSeed,CrvCoincidenceCluster,MVAResult>); omit to disable")};
        // MC sub-config (requires mc.fill also be true)
        struct MCConfig {
          using Name=fhicl::Name;
          using Comment=fhicl::Comment;
          fhicl::Atom<bool>          fill              {Name("fill"),               Comment("Master switch for CRV MC branches; mc.fill must also be true")};
          fhicl::Atom<art::InputTag> coincidenceMCsTag {Name("coincidenceMCsTag"),  Comment("Tag for CrvCoincidenceClusterMC collection")};
          fhicl::Atom<art::InputTag> digiMCsTag        {Name("digiMCsTag"),         Comment("Tag for CrvDigiMC collection")};
          fhicl::Atom<art::InputTag> assnsTag          {Name("assnsTag"),            Comment("Tag for CrvCoincidenceClusterMCAssns")};
        };
        fhicl::Table<MCConfig> mc{Name("mc"), Comment("CRV MC filling options")};
      };

      struct Config {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;

        // General control
        fhicl::Atom<int> diag{Name("diagLevel")};
        fhicl::Atom<int> debug{Name("debugLevel")};
        fhicl::Atom<int> splitlevel{Name("splitlevel")};
        fhicl::Atom<int> buffsize{Name("buffsize")};
        fhicl::Atom<bool> hastrks{Name("hasTracks"), Comment("Require >=1 tracks to fill tuple")};
        fhicl::Atom<bool> hascrv{Name("hasCRV"), Comment("Require CRV information to fill tuple")};
        // General event info
        fhicl::Atom<art::InputTag> rctag{Name("RecoCountTag"), Comment("RecoCount")};
        fhicl::Atom<art::InputTag> PBITag{Name("PBITag"), Comment("Tag for ProtonBunchIntensity object")};
        fhicl::Atom<art::InputTag> PBTTag{Name("PBTTag"), Comment("Tag for ProtonBunchTime object")};
        fhicl::Atom<art::InputTag> EWMTag{Name("EWMTag"), Comment("Tag for EventWindowMarker object")};
        // Trigger
        fhicl::Atom<bool> filltrig{Name("FillTriggerInfo")};
        fhicl::Atom<std::string> trigProcessName{Name("TriggerProcessName"), Comment("Process name for Trigger")};
        fhicl::Atom<std::string> trigpathsuffix{Name("TriggerPathSuffix"), "_trigger"};
        // Fit type
        fhicl::Atom<std::string> fittype{Name("FitType"), Comment("Type of track fit: LoopHelix, CentralHelix, KinematicLine, or Unknown"), "Unknown"};
        // ── Subsystems ────────────────────────────────────────────────────────
        fhicl::Table<EventMCConfig>     mc          {Name("mc"),          Comment("Event-level MC config: gates tracker+CRV MC and provides shared event MC products")};
        fhicl::Table<TrkConfig>         trk         {Name("trk"),         Comment("Tracker subsystem config")};
        fhicl::Table<CaloConfig>        calo        {Name("calo"),        Comment("Calorimeter subsystem config")};
        fhicl::Table<CRVConfig>         crv         {Name("crv"),         Comment("CRV subsystem config")};
        fhicl::Table<HelixConfig>       helices     {Name("helices"),     Comment("Helix seed branches config")};
        fhicl::Table<TimeClusterConfig> timeclusters{Name("timeclusters"),Comment("Time cluster branch config")};
        fhicl::Table<MCStepsConfig>     mcsteps     {Name("mcsteps"),     Comment("MC step collection branches config")};
      };
      typedef art::EDAnalyzer::Table<Config> Parameters;

      explicit EventNtupleMaker(const Parameters& conf);
      virtual ~EventNtupleMaker() { }

      void beginJob() override;
      void beginSubRun(const art::SubRun & subrun ) override;
      void analyze(const art::Event& e) override;

    private:

      Config _conf;
      std::vector<TrkFitConfig> _allTrkFitBranches; // configurations for all track fit branches
      // main TTree
      TTree* _ntuple;
      TH1I* _hVersion;
      TH1I* _hProcEvents;
      // general event info branch
      EventInfo _einfo;
      EventInfoMC _einfomc;
      art::InputTag _recoCountTag, _PBITag, _PBTTag, _EWMTag;
      art::Handle<mu2e::EventWindowMarker> _ewmh;
      // track control
      bool _hastrks;
      bool _hascrv;
      // hit counting
      HitCount _hcnt;
      // track counting
      TrkCount _tcnt;
      // track branches (inputs)
      std::vector<art::Handle<KalSeedPtrCollection> > _allKSPCHs;
      // track branches (outputs)
      std::map<TrkFitBranchIndex, std::vector<TrkInfo>> _allTIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<TrkSegInfo>>> _allTSIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<LoopHelixInfo>>> _allLHIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<CentralHelixInfo>>> _allCHIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<KinematicLineInfo>>> _allKLIs;
      std::map<TrkFitBranchIndex, std::vector<TrkCaloHitInfo>> _allTCHIs;
      // quality branches (inputs)
      std::vector<std::vector<art::Handle<RecoQualCollection> > > _allRQCHs;
      std::vector<std::vector<art::Handle<MVAResultCollection> >> _allTrkQualCHs;
      std::vector<std::vector<art::Handle<MVAResultCollection> >> _allTrkPIDCHs;
      // quality branches (outputs)
      std::vector<RecoQualInfo> _allRQIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<MVAResultInfo>>> _allTrkQualResults;
      std::map<TrkFitBranchIndex, std::vector<std::vector<MVAResultInfo>>> _allTrkPIDResults;
      // trigger information
      unsigned _trigbits;
      std::map<size_t,unsigned> _tmap; // map between path and trigger ID.  ID should come from trigger itself FIXME!
      TrigInfo _triggerResults;
      // cached optional tracker MC tags
      std::vector<art::InputTag> _extraMCStepTags;
      std::vector<art::Handle<StepPointMCCollection>> _extraMCStepCollections;
      std::map<TrkFitBranchIndex, std::map<StepCollIndex, std::vector<MCStepInfos>>> _extraMCStepInfos;
      std::map<TrkFitBranchIndex, std::map<StepCollIndex, std::vector<MCStepSummaryInfo>>> _extraMCStepSummaryInfos;
      art::InputTag _surfaceStepsTag;
      std::map<TrkFitBranchIndex, std::vector<std::vector<SurfaceStepInfo>>> _surfaceStepInfos;
      art::Handle<SurfaceStepCollection> _surfaceStepsHandle;
      std::vector<art::InputTag> _stepPointMCTags;
      std::vector<art::Handle<StepPointMCCollection>> _stepPointMCCollections;
      std::map<StepCollIndex, MCStepInfos> _stepPointMCInfos;
      // MC truth handles
      art::Handle<PrimaryParticle> _pph;
      art::Handle<KalSeedMCAssns> _ksmcah;
      art::Handle<SimParticleCollection> _simParticles;
      art::Handle<MCTrajectoryCollection> _mcTrajectories;
      // tracker MC truth branches (outputs)
      std::map<TrkFitBranchIndex, std::vector<TrkInfoMC>> _allMCTIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<SimInfo>>> _allMCSimTIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<MCStepInfo>>> _allMCVDInfos;
      art::Handle<CaloClusterMCCollection> _ccmcch;
      art::Handle<CaloHitMCCollection> _chmcch;
      std::map<TrkFitBranchIndex, std::vector<CaloClusterInfoMC>> _allMCTCHIs;
      // hit level info branches
      std::map<TrkFitBranchIndex, std::vector<std::vector<TrkStrawHitInfo>>> _allTSHIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<TrkStrawHitCalibInfo>>> _allTSHCIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<TrkStrawMatInfo>>> _allTSMIs;
      std::map<TrkFitBranchIndex, std::vector<std::vector<TrkStrawHitInfoMC>>> _allTSHIMCs;
      // time cluster branch
      art::Handle<TimeClusterCollection> _tcsHandle;
      std::vector<EventNtupleTimeClusterInfo> _tcIs;
      // event weights
      std::vector<art::Handle<EventWeight> > _wtHandles;
      EventWeightInfo _wtinfo;
      // Calorimeter
      art::Handle<CaloClusterCollection> _caloClusters;
      art::Handle<CaloHitCollection> _caloHits;
      art::Handle<CaloRecoDigiCollection> _caloRecoDigis;
      art::Handle<CaloDigiCollection> _caloDigis;
      art::Handle<CaloShowerSimCollection> _caloShowerSim;
      std::vector<CaloClusterInfo> _caloCIs;
      std::vector<CaloHitInfo> _caloHIs;
      std::vector<CaloRecoDigiInfo> _caloRDIs;
      std::vector<CaloDigiInfo> _caloDIs;
      std::vector<CaloDigiMCInfo> _caloDigiMCIs;
      // Calorimeter MC
      std::vector<CaloClusterInfoMC> _caloCIMCs;
      std::vector<CaloHitInfoMC> _caloHIMCs;
      std::vector<SimInfo> _caloSIMCs;
      std::vector<SimInfo> _caloDigiSIMCs;
      // CRV (inputs)
      std::map<TrkFitBranchIndex, std::vector<art::Handle<BestCrvAssns>>> _allBestCrvAssns;
      art::Handle<CrvCoincidenceClusterMCAssns>      _crvMCAssns;
      art::Handle<CrvCoincidenceClusterCollection>   _crvCoincidences;
      art::Handle<CrvCoincidenceClusterMCCollection> _crvCoincidenceMCs;
      art::Handle<CrvRecoPulseCollection>            _crvRecoPulses;
      art::Handle<CrvDigiMCCollection>               _crvDigiMCs;
      art::Handle<CrvDigiCollection>                 _crvDigis;
      art::Handle<CrvStepCollection>                 _crvSteps;
      // CRV inference
      bool _fillCrvInference;
      art::InputTag _crvInferenceTag;
      std::vector<std::vector<MVAResultInfo>> _crvInference;
      // CRV (output)
      std::vector<CrvHitInfoReco> _crvcoincs;
      std::map<TrkFitBranchIndex, std::vector<CrvHitInfoReco>> _allBestCrvs;
      std::vector<CrvHitInfoMC> _crvcoincsmc;
      std::map<TrkFitBranchIndex, std::vector<CrvHitInfoMC>> _allBestCrvMCs;
      CrvSummaryReco _crvsummary;
      CrvSummaryMC   _crvsummarymc;
      std::vector<CrvPlaneInfoMC> _crvcoincsmcplane;
      std::vector<CrvPulseInfoReco> _crvpulses;
      std::vector<CrvWaveformInfo> _crvdigis;
      std::vector<CrvHitInfoMC> _crvpulsesmc;
      std::vector<CrvHitInfoReco> _crvrecoinfo;
      // helices
      std::vector<HelixInfo> _hinfos;
      // struct helpers
      InfoStructHelper _infoStructHelper;
      InfoMCStructHelper _infoMCStructHelper;
      CrvInfoHelper _crvHelper;
      // branch structure
      Int_t _buffsize, _splitlevel;
      enum FType{Unknown=0,LoopHelix,CentralHelix,KinematicLine};
      FType _ftype = Unknown;
      std::vector<std::string> fitNames = {"Unknown", "LoopHelix","CentralHelix","KinematicLine"};
      // for trigger branch:
      bool firstEvent = true;

      // ── Gating helpers ─────────────────────────────────────────────────────
      // Event-level MC gate (simParticles, mcTrajectories, primaryParticle).
      // Also gates tracker MC and CRV MC.
      bool fillEventMC() const { return _conf.mc().fill(); }

      // Tracker MC: both the global event MC gate and the tracker MC gate must
      // be true, plus the per-branch flag.
      bool fillTrkMC(const TrkFitOptConfig& opt) const {
        return _conf.trk().fill() && _conf.mc().fill() && _conf.trk().mc().fill() && opt.fillmc();
      }

      // Calorimeter MC: independent of fillEventMC() — calo MC products do not
      // require simParticles/primaryParticle so calo-only jobs can enable this
      // while keeping mc.fill false.
      bool fillCaloMC()            const { return _conf.calo().fill() && _conf.calo().mc().fill() && _conf.mc().fill(); }
      bool fillCaloTrackMatchMC()  const { return fillCaloMC() && _conf.calo().mc().fillTrackMatch(); }
      bool fillCaloClsMC()         const { return fillCaloMC() && _conf.calo().mc().fillClusters(); }
      bool fillCaloHitsMC()        const { return fillCaloMC() && _conf.calo().mc().fillHits(); }
      bool fillCaloSimMC()         const { return fillCaloMC() && _conf.calo().mc().fillSim(); }
      bool fillCaloDigisMC()       const { return fillCaloMC() && _conf.calo().mc().fillDigis(); }
      bool fillCaloDigiSimMC()     const { return fillCaloMC() && _conf.calo().mc().fillDigiSim(); }

      // CRV MC: needs global event MC gate (CRV uses mcTrajectories/primaryParticle).
      bool fillCRVMC() const {
        return _conf.mc().fill() && _conf.crv().fill() && _conf.crv().mc().fill();
      }

      // helper functions
      void fillEventInfo(const art::Event& event);
      void fillTriggerBranch(const art::Event& event, std::string const& process, bool firstEvent);
      void resetTrackBranches();
      void fillTrackBranches(const art::Handle<KalSeedPtrCollection>& kspch, TrkFitBranchIndex i_trk_fit_branch, size_t i_kseedptr);

      template <typename T, typename TI, typename TIA>
        std::vector<art::Handle<T> > createSpecialBranch(const art::Event& event, const std::string& branchname,
            std::vector<art::Handle<T> >& handles, TI& infostruct, TIA& array, bool make_individual_branches = false, const std::string& selection = "");

  };

  EventNtupleMaker::EventNtupleMaker(const Parameters& conf):
    art::EDAnalyzer(conf),
    _conf(conf()),
    _recoCountTag(conf().rctag()),
    _PBITag(conf().PBITag()),
    _PBTTag(conf().PBTTag()),
    _EWMTag(conf().EWMTag()),
    _hastrks(conf().hastrks()),
    _hascrv(conf().hascrv()),
    _infoMCStructHelper(conf().mc().infoMCStructHelper()),
    _buffsize(conf().buffsize()),
    _splitlevel(conf().splitlevel())
  {
    _fillCrvInference = conf().crv().inferenceTag(_crvInferenceTag);

    // decode fit type
    for(size_t ifit = 0; ifit < fitNames.size(); ++ifit){
      if(_conf.fittype() == fitNames[ifit]){
        _ftype = (FType)ifit;
        break;
      }
    }

    // resolve optional MC step tags from top-level mcsteps config
    _conf.mcsteps().extraMCStepTags(_extraMCStepTags);
    _conf.mcsteps().surfaceStepsTag(_surfaceStepsTag);
    _conf.mcsteps().stepPointMCTags(_stepPointMCTags);

    // populate branch list from trk.branches
    for(const auto& trk_fit_cfg : _conf.trk().fits()){
      _allTrkFitBranches.push_back(trk_fit_cfg);
    }

    // create per-branch storage (skip disabled branches)
    for (TrkFitBranchIndex i_trk_fit_branch = 0; i_trk_fit_branch < _allTrkFitBranches.size(); ++i_trk_fit_branch) {
      const auto& i_trkFitConfig = _allTrkFitBranches.at(i_trk_fit_branch);
      if (!i_trkFitConfig.fill()) continue;

      _allTIs[i_trk_fit_branch]       = {};
      _allTSIs[i_trk_fit_branch]      = {};
      _allLHIs[i_trk_fit_branch]      = {};
      _allCHIs[i_trk_fit_branch]      = {};
      _allKLIs[i_trk_fit_branch]      = {};
      _allMCVDInfos[i_trk_fit_branch] = {};
      _allTCHIs[i_trk_fit_branch]     = {};
      _allMCTIs[i_trk_fit_branch]     = {};
      _allMCSimTIs[i_trk_fit_branch]  = {};

      if(fillCaloTrackMatchMC()){
        _allMCTCHIs[i_trk_fit_branch] = {};
      }

      _allRQIs.push_back(RecoQualInfo{});
      _allTSHIs[i_trk_fit_branch]   = {};
      _allTSHCIs[i_trk_fit_branch]  = {};
      _allTSMIs[i_trk_fit_branch]   = {};
      _allTSHIMCs[i_trk_fit_branch] = {};

      _allTrkQualResults[i_trk_fit_branch].resize(i_trkFitConfig.trkQualTags().size());
      _allTrkPIDResults[i_trk_fit_branch].resize(i_trkFitConfig.trkPIDTags().size());

      for (StepCollIndex ixt = 0; ixt < _extraMCStepTags.size(); ++ixt) {
        _extraMCStepInfos[i_trk_fit_branch][ixt]        = {};
        _extraMCStepSummaryInfos[i_trk_fit_branch][ixt] = {};
      }
      if (!_surfaceStepsTag.empty()) {
        _surfaceStepInfos[i_trk_fit_branch] = {};
      }
    }

    // global (non-per-branch) MC step storage
    for (StepCollIndex icoll = 0; icoll < _stepPointMCTags.size(); ++icoll) {
      _stepPointMCInfos[icoll] = MCStepInfos{};
    }
  }

  void EventNtupleMaker::beginJob( ){
    art::ServiceHandle<art::TFileService> tfs;
    // create TTree
    _ntuple=tfs->make<TTree>("ntuple","Mu2e Event Ntuple");
    _hVersion = tfs->make<TH1I>("version", "version number",3,0,3);
    _hVersion->GetXaxis()->SetBinLabel(1, "major"); _hVersion->SetBinContent(1, 6);
    _hVersion->GetXaxis()->SetBinLabel(2, "minor"); _hVersion->SetBinContent(2, 11);
    _hVersion->GetXaxis()->SetBinLabel(3, "patch"); _hVersion->SetBinContent(3, 2);
    _hProcEvents = tfs->make<TH1I>("n_proc_events", "number of processed events", 1,0,1);
    // event info branch
    _ntuple->Branch("evtinfo",&_einfo,_buffsize,_splitlevel);
    if (fillEventMC()) {
      _ntuple->Branch("evtinfomc",&_einfomc,_buffsize,_splitlevel);
    }
    // hit counting branch
    _ntuple->Branch("hitcount",&_hcnt);
    // track counting branches
    for (TrkFitBranchIndex i_trk_fit_branch = 0; i_trk_fit_branch < _allTrkFitBranches.size(); ++i_trk_fit_branch) {
      if (!_allTrkFitBranches.at(i_trk_fit_branch).fill()) continue;
      std::string leafname = _allTrkFitBranches.at(i_trk_fit_branch).branchname();
      _ntuple->Branch(("tcnt.n"+leafname).c_str(),&_tcnt._counts[i_trk_fit_branch]);
    }

    // create all track branches
    if (_conf.trk().fill()) {
      for (TrkFitBranchIndex i_trk_fit_branch = 0; i_trk_fit_branch < _allTrkFitBranches.size(); ++i_trk_fit_branch) {
        const TrkFitConfig& i_trkFitConfig = _allTrkFitBranches.at(i_trk_fit_branch);
        if (!i_trkFitConfig.fill()) continue;
        std::string branch = i_trkFitConfig.branchname();
        _ntuple->Branch((branch+".").c_str(),&_allTIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
        _ntuple->Branch((branch+"segs.").c_str(),&_allTSIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
        // add traj-specific branches
        if(_ftype == LoopHelix)    _ntuple->Branch((branch+"segpars_lh.").c_str(),&_allLHIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
        if(_ftype == CentralHelix) _ntuple->Branch((branch+"segpars_ch.").c_str(),&_allCHIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
        if(_ftype == KinematicLine) _ntuple->Branch((branch+"segpars_kl.").c_str(),&_allKLIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
        // TrkCaloHit: currently only 1
        _ntuple->Branch((branch+"calohit.").c_str(),&_allTCHIs.at(i_trk_fit_branch));
        for (size_t i_trkQualTag = 0; i_trkQualTag < i_trkFitConfig.trkQualTags().size(); ++i_trkQualTag) {
          std::string branchname = "qual";
          if (i_trkQualTag > 0) branchname += std::to_string(i_trkQualTag+1);
          _ntuple->Branch((branch+branchname+".").c_str(),&_allTrkQualResults.at(i_trk_fit_branch).at(i_trkQualTag),_buffsize,_splitlevel);
        }
        for (size_t i_trkPIDTag = 0; i_trkPIDTag < i_trkFitConfig.trkPIDTags().size(); ++i_trkPIDTag) {
          std::string branchname = "pid";
          if (i_trkPIDTag > 0) branchname += std::to_string(i_trkPIDTag+1);
          _ntuple->Branch((branch+branchname+'.').c_str(),&_allTrkPIDResults.at(i_trk_fit_branch).at(i_trkPIDTag),_buffsize,_splitlevel);
        }
        // hit-level branches (global trk.fillHits AND per-branch options.fillHits both required)
        if(_conf.diag() > 1 || (_conf.trk().fillHits() && i_trkFitConfig.options().fillhits())){
          _ntuple->Branch((branch+"hits.").c_str(),&_allTSHIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
          if (_conf.trk().fillHitCalibs())
            _ntuple->Branch((branch+"hitcalibs.").c_str(),&_allTSHCIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
          _ntuple->Branch((branch+"mats.").c_str(),&_allTSMIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
        }
        // tracker MC branches
        if(fillTrkMC(i_trkFitConfig.options())){
          _ntuple->Branch((branch+"mc.").c_str(),&_allMCTIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
          _ntuple->Branch((branch+"mcsim.").c_str(),&_allMCSimTIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
          _ntuple->Branch((branch+"mcvd.").c_str(),&_allMCVDInfos.at(i_trk_fit_branch),_buffsize,_splitlevel);
          if(fillCaloTrackMatchMC())
            _ntuple->Branch((branch+"calohitmc.").c_str(),&_allMCTCHIs.at(i_trk_fit_branch),_buffsize,_splitlevel);
          if(_conf.diag() > 1 || (_conf.trk().fillHits() && i_trkFitConfig.options().fillhits())){
            _ntuple->Branch((branch+"hitsmc.").c_str(),&_allTSHIMCs.at(i_trk_fit_branch),_buffsize,_splitlevel);
          }
          // extra MCStep branches per track type
          for(size_t ixtra = 0; ixtra < _extraMCStepTags.size(); ++ixtra) {
            auto const& tag = _extraMCStepTags[ixtra];
            auto inst = tag.instance();
            _ntuple->Branch((branch+"mcsic_"+inst+".").c_str(),&_extraMCStepInfos[i_trk_fit_branch][ixtra],_buffsize,_splitlevel);
            _ntuple->Branch((branch+"mcssi_"+inst+".").c_str(),&_extraMCStepSummaryInfos[i_trk_fit_branch][ixtra],_buffsize,_splitlevel);
          }
          if(!_surfaceStepsTag.empty()){
            _ntuple->Branch((branch+"segsmc.").c_str(),&_surfaceStepInfos[i_trk_fit_branch],_buffsize,_splitlevel);
          }
        }
      }
    }

    // Time clusters
    if(_conf.timeclusters().fill()) {
      _ntuple->Branch("timeclusters.",&_tcIs,_buffsize,_splitlevel);
    }

    // ── Calorimeter reco branches ──────────────────────────────────────────
    if(_conf.calo().fill() && _conf.calo().fillClusters()){
      _ntuple->Branch("caloclusters.",&_caloCIs,_buffsize,_splitlevel);
    }
    if(_conf.calo().fill() && _conf.calo().fillHits()){
      _ntuple->Branch("calohits.",&_caloHIs,_buffsize,_splitlevel);
    }
    if(_conf.calo().fill() && _conf.calo().fillRecoDigis()){
      _ntuple->Branch("calorecodigis.",&_caloRDIs,_buffsize,_splitlevel);
    }
    if(_conf.calo().fill() && _conf.calo().fillDigis()){
      _ntuple->Branch("calodigis.",&_caloDIs,_buffsize,_splitlevel);
    }
    // ── Calorimeter MC branches ────────────────────────────────────────────
    if(fillCaloDigisMC()){
      _ntuple->Branch("calodigismc.",&_caloDigiMCIs,_buffsize,_splitlevel);
    }
    if(fillCaloDigiSimMC()){
      _ntuple->Branch("calodigisim.",&_caloDigiSIMCs,_buffsize,_splitlevel);
    }
    if(fillCaloClsMC()){
      _ntuple->Branch("caloclustersmc.",&_caloCIMCs,_buffsize,_splitlevel);
    }
    if(fillCaloHitsMC()){
      _ntuple->Branch("calohitsmc.",&_caloHIMCs,_buffsize,_splitlevel);
    }
    if(fillCaloSimMC()){
      _ntuple->Branch("calomcsim.",&_caloSIMCs,_buffsize,_splitlevel);
    }

    // ── CRV reco branches ──────────────────────────────────────────────────
    if(_conf.crv().fill() && _conf.crv().fillCoincs()) {
      _ntuple->Branch("crvsummary",&_crvsummary,_buffsize,_splitlevel);
      _ntuple->Branch("crvcoincs.",&_crvcoincs,_buffsize,_splitlevel);
      if(fillCRVMC()){
        _ntuple->Branch("crvsummarymc",&_crvsummarymc,_buffsize,_splitlevel);
        _ntuple->Branch("crvcoincsmc.",&_crvcoincsmc,_buffsize,_splitlevel);
        _ntuple->Branch("crvcoincsmcplane.",&_crvcoincsmcplane,_buffsize,_splitlevel);
      }
    }
    if(_conf.crv().fill() && _conf.crv().fillPulses()) {
      _ntuple->Branch("crvpulses.",&_crvpulses,_buffsize,_splitlevel);
      if(fillCRVMC()) _ntuple->Branch("crvpulsesmc.",&_crvpulsesmc,_buffsize,_splitlevel);
    }
    if(_conf.crv().fill() && _conf.crv().fillDigis()) {
      _ntuple->Branch("crvdigis.",&_crvdigis,_buffsize,_splitlevel);
    }
    // CRV cosmic inference
    if(_fillCrvInference) {
      _ntuple->Branch("crvcosmic.",&_crvInference,_buffsize,_splitlevel);
    }
    // helix info
    if(_conf.helices().fill()) _ntuple->Branch("helices.",&_hinfos,_buffsize,_splitlevel);

    // global MC step branches (all steps, not per-track)
    if(fillEventMC()){
      for(size_t icoll = 0; icoll < _stepPointMCTags.size(); ++icoll) {
        auto const& tag = _stepPointMCTags[icoll];
        auto inst = tag.instance();
        _ntuple->Branch(("mcsteps_"+inst+".").c_str(),&_stepPointMCInfos[icoll],_buffsize,_splitlevel);
      }
    }
  }

  void EventNtupleMaker::beginSubRun(const art::SubRun & subrun ) {
    _infoStructHelper.updateSubRun();
  }

  void EventNtupleMaker::analyze(const art::Event& event) {
    // reset event level structs
    _einfo.reset();
    _einfomc.reset();
    _hcnt.reset();
    _tcnt.reset();
    _wtinfo.reset();

    // clear the vector branches
    _hinfos.clear();

    // update timing maps for MC
    if(fillEventMC()){
      _infoMCStructHelper.updateEvent(event);
    }
    // fill event level info
    fillEventInfo(event);

    // create event weight branch
    std::vector<art::Handle<EventWeight> > eventWeightHandles;
    _wtHandles = createSpecialBranch(event, "evtwt", eventWeightHandles, _wtinfo, _wtinfo._weights, false);

    std::string process = _conf.trigProcessName();
    _allKSPCHs.clear();
    _allRQCHs.clear();
    _allBestCrvAssns.clear();
    _allTrkQualCHs.clear();
    _allTrkPIDCHs.clear();

    art::Handle<KalHelixAssns> khaH;
    if(_conf.helices().fill()){
      TrkFitConfig i_trkFitConfig = _allTrkFitBranches.at(0);
      art::InputTag kalSeedInputTag = i_trkFitConfig.input();
      event.getByLabel(kalSeedInputTag,khaH);
    }

    if (_conf.trk().fill()) {
      for (TrkFitBranchIndex i_trk_fit_branch = 0; i_trk_fit_branch < _allTrkFitBranches.size(); ++i_trk_fit_branch) {
        TrkFitConfig i_trkFitConfig = _allTrkFitBranches.at(i_trk_fit_branch);
        if (!i_trkFitConfig.fill()) {
          // push empty placeholders to keep index alignment
          _allKSPCHs.push_back(art::Handle<KalSeedPtrCollection>());
          _allTrkQualCHs.emplace_back();
          _allRQCHs.push_back({});
          _allTrkPIDCHs.emplace_back();
          continue;
        }
        art::Handle<KalSeedPtrCollection> kalSeedPtrCollHandle;
        art::InputTag kalSeedPtrInputTag = i_trkFitConfig.input();
        event.getByLabel(kalSeedPtrInputTag,kalSeedPtrCollHandle);
        _allKSPCHs.push_back(kalSeedPtrCollHandle);

        std::vector<art::Handle<MVAResultCollection>> trkQualCollHandles;
        for (const auto& i_trkQualTag : i_trkFitConfig.trkQualTags()) {
          art::Handle<MVAResultCollection> trkQualCollHandle;
          event.getByLabel(i_trkQualTag,trkQualCollHandle);
          trkQualCollHandles.push_back(trkQualCollHandle);
        }
        _allTrkQualCHs.emplace_back(trkQualCollHandles);

        std::vector<art::Handle<RecoQualCollection> > recoQualCollHandles;
        std::vector<art::Handle<RecoQualCollection> > selectedRQCHs;
        selectedRQCHs = createSpecialBranch(event, i_trkFitConfig.branchname()+"qual", recoQualCollHandles, _allRQIs.at(i_trk_fit_branch), _allRQIs.at(i_trk_fit_branch)._qualsAndCalibs, true);
        for (const auto& i_selectedRQCH : selectedRQCHs) {
          if (i_selectedRQCH->size() != kalSeedPtrCollHandle->size()) {
            throw cet::exception("TrkAna") << "Sizes of KalSeedPtrCollection and this RecoQualCollection are inconsistent (" << kalSeedPtrCollHandle->size() << " and " << i_selectedRQCH->size() << " respectively)";
          }
        }
        _allRQCHs.push_back(selectedRQCHs);

        std::vector<art::Handle<MVAResultCollection>> trkPIDCollHandles;
        for (const auto& i_trkPIDTag : i_trkFitConfig.trkPIDTags()) {
          art::Handle<MVAResultCollection> trkPIDCollHandle;
          event.getByLabel(i_trkPIDTag,trkPIDCollHandle);
          trkPIDCollHandles.push_back(trkPIDCollHandle);
        }
        _allTrkPIDCHs.emplace_back(trkPIDCollHandles);
      }
    }

    // trigger information
    if(_conf.filltrig()){
      fillTriggerBranch(event, process, firstEvent);
      firstEvent=false;
    }

    // load event-level MC products (tracker + CRV MC)
    if(fillEventMC()) {
      event.getByLabel(_conf.mc().primaryParticleTag(),_pph);
      event.getByLabel(_conf.trk().mc().kalSeedMCAssns(),_ksmcah);
      event.getByLabel(_conf.mc().simParticlesTag(),_simParticles);
      event.getByLabel(_conf.mc().mcTrajectoriesTag(),_mcTrajectories);
    }
    // load calo MC products (independent of fillEventMC)
    if(fillCaloClsMC() || fillCaloTrackMatchMC()){
      event.getByLabel(_conf.calo().mc().clusterMCTag(),_ccmcch);
    }
    if(fillCaloHitsMC()){
      event.getByLabel(_conf.calo().mc().hitMCTag(),_chmcch);
    }

    // fill track counts
    if (_conf.trk().fill()) {
      for (TrkFitBranchIndex i_trk_fit_branch = 0; i_trk_fit_branch < _allTrkFitBranches.size(); ++i_trk_fit_branch) {
        if (!_allTrkFitBranches.at(i_trk_fit_branch).fill()) continue;
        _tcnt._counts[i_trk_fit_branch] = (_allKSPCHs.at(i_trk_fit_branch))->size();
      }
    }

    // find extra MCStep collections
    _extraMCStepCollections.clear();
    for(size_t ixt = 0; ixt < _extraMCStepTags.size(); ixt++){
      art::Handle<StepPointMCCollection> mcstepch;
      event.getByLabel(_extraMCStepTags[ixt],mcstepch);
      _extraMCStepCollections.push_back(mcstepch);
    }
    event.getByLabel(_surfaceStepsTag,_surfaceStepsHandle);

    // find global MCStep collections
    _stepPointMCCollections.clear();
    for(size_t icoll = 0; icoll < _stepPointMCTags.size(); icoll++){
      art::Handle<StepPointMCCollection> mcstepch;
      event.getByLabel(_stepPointMCTags[icoll],mcstepch);
      _stepPointMCCollections.push_back(mcstepch);
    }

    // loop through all track types
    unsigned ntrks(0);
    for (TrkFitBranchIndex i_trk_fit_branch = 0; i_trk_fit_branch < _allTrkFitBranches.size(); ++i_trk_fit_branch) {
      TrkFitConfig i_trkFitConfig = _allTrkFitBranches.at(i_trk_fit_branch);
      if (!i_trkFitConfig.fill()) continue;

      _allTIs.at(i_trk_fit_branch).clear();
      _allTSIs.at(i_trk_fit_branch).clear();
      _allLHIs.at(i_trk_fit_branch).clear();
      _allCHIs.at(i_trk_fit_branch).clear();
      _allKLIs.at(i_trk_fit_branch).clear();
      _allTCHIs.at(i_trk_fit_branch).clear();

      _allTSHIs.at(i_trk_fit_branch).clear();
      _allTSHCIs.at(i_trk_fit_branch).clear();
      _allTSMIs.at(i_trk_fit_branch).clear();
      _allTSHIMCs.at(i_trk_fit_branch).clear();

      _allMCTIs.at(i_trk_fit_branch).clear();
      _allMCVDInfos.at(i_trk_fit_branch).clear();
      _allMCSimTIs.at(i_trk_fit_branch).clear();

      for (size_t i_trkQualTag = 0; i_trkQualTag < i_trkFitConfig.trkQualTags().size(); ++i_trkQualTag) {
        _allTrkQualResults.at(i_trk_fit_branch).at(i_trkQualTag).clear();
      }
      for (size_t i_trkPIDTag = 0; i_trkPIDTag < i_trkFitConfig.trkPIDTags().size(); ++i_trkPIDTag) {
        _allTrkPIDResults.at(i_trk_fit_branch).at(i_trkPIDTag).clear();
      }
      for (StepCollIndex ixt = 0; ixt < _extraMCStepTags.size(); ++ixt) {
        _extraMCStepInfos.at(i_trk_fit_branch).at(ixt).clear();
        _extraMCStepSummaryInfos.at(i_trk_fit_branch).at(ixt).clear();
      }
      _surfaceStepInfos.at(i_trk_fit_branch).clear();
      for (StepCollIndex icoll = 0; icoll < _stepPointMCTags.size(); ++icoll) {
        _stepPointMCInfos.at(icoll).clear();
      }

      if(fillCaloTrackMatchMC()) { _allMCTCHIs.at(i_trk_fit_branch).clear(); }

      if (_conf.trk().fill()) {
        const auto& kseedptr_coll_h = _allKSPCHs.at(i_trk_fit_branch);
        const auto& kseedptr_coll = *kseedptr_coll_h;
        for (size_t i_kseedptr = 0; i_kseedptr < kseedptr_coll.size(); ++i_kseedptr) {
          resetTrackBranches();
          fillTrackBranches(kseedptr_coll_h, i_trk_fit_branch, i_kseedptr);
          if(_conf.helices().fill()){
            auto const& khassns = khaH.product();
            auto hptr = (*khassns)[i_kseedptr].second;
            _infoStructHelper.fillHelixInfo(hptr, _hinfos);
          }
          ntrks++;
        }
      }
    }

    // Time clusters
    if(_conf.timeclusters().fill()) {
      _tcIs.clear();
      event.getByLabel(_conf.timeclusters().tag(),_tcsHandle);
      if(_tcsHandle.isValid()) {
        for(const auto& tc : *(_tcsHandle)) {
          _infoStructHelper.fillTimeClusterInfo(tc, _tcIs);
        }
      }
    }

    // ── Calorimeter ────────────────────────────────────────────────────────
    if(fillCaloClsMC())    { _caloCIMCs.clear(); }
    if(fillCaloHitsMC())   { _caloHIMCs.clear(); }
    if(fillCaloSimMC())    { _caloSIMCs.clear(); }
    if(fillCaloDigiSimMC()){ _caloDigiSIMCs.clear(); }
    if(fillCaloDigisMC())  { _caloDigiMCIs.clear(); }
    if(_conf.calo().fill() && _conf.calo().fillClusters()) { _caloCIs.clear(); }
    if(_conf.calo().fill() && _conf.calo().fillHits())     { _caloHIs.clear(); }
    if(_conf.calo().fill() && _conf.calo().fillRecoDigis()){ _caloRDIs.clear(); }
    if(_conf.calo().fill() && _conf.calo().fillDigis())    { _caloDIs.clear(); }

    // Calo MC digi branches (need showerSim)
    if(fillCaloDigisMC() || fillCaloDigiSimMC()) {
      event.getByLabel(_conf.calo().mc().showerSimTag(),_caloShowerSim);
    }
    if(fillCaloDigisMC()){
      if(_caloShowerSim.isValid()){
        for(const auto& showerSim : *_caloShowerSim.product()){
          _infoMCStructHelper.fillCaloDigiMCInfo(showerSim,_caloDigiMCIs);
        }
      }
    }
    if(fillCaloHitsMC()){
      for(const auto& hitmc : *_chmcch.product()){
        _infoMCStructHelper.fillCaloHitInfoMC(hitmc,_caloHIMCs);
      }
    }
    if(fillCaloClsMC()){
      for(const auto& clustermc : *_ccmcch.product()){
        _infoMCStructHelper.fillCaloClusterInfoMC(clustermc,_caloCIMCs);
        if(fillCaloHitsMC()){
          for(const auto& hitmc : clustermc.caloHitMCs()){
            for(uint hitIdx = 0; hitIdx < _caloHIMCs.size(); hitIdx++){
              if(hitmc && _caloHIMCs[hitIdx] == *hitmc){
                _caloHIMCs[hitIdx].clusterIdx_ = _caloCIMCs.size()-1;
                _caloCIMCs.back().hits_.push_back(hitIdx);
                break;
              }
            }
          }
          if(int(_caloCIMCs.back().hits_.size()) != _caloCIMCs.back().nhits){
            throw cet::exception("EventNtuple") << "Could not find one or all CaloHitMCs linked to CaloClusterMC " << _caloCIMCs.size()-1 << "\n";
          }
        }
      }
    }
    if(fillCaloSimMC()){
      for(const auto& clustermc : *_ccmcch.product()){
        _infoMCStructHelper.fillCaloSimInfos(clustermc,_caloSIMCs);
      }
    }
    if(fillCaloDigiSimMC()){
      if(_caloShowerSim.isValid()){
        for(const auto& showerSim : *_caloShowerSim.product()){
          _infoMCStructHelper.fillCaloDigiSimInfos(showerSim,_caloDigiSIMCs);
        }
      }
    }

    // Calorimeter reco branches
    if(_conf.calo().fill() && _conf.calo().fillDigis()){
      event.getByLabel(_conf.calo().digisTag(),_caloDigis);
      for(const auto& digi : *_caloDigis.product()){
        _infoStructHelper.fillCaloDigiInfo(digi,_caloDIs);
      }
    }
    if(_conf.calo().fill() && _conf.calo().fillRecoDigis()){
      event.getByLabel(_conf.calo().recoDigisTag(),_caloRecoDigis);
      for(const auto& recodigi : *_caloRecoDigis.product()){
        _infoStructHelper.fillCaloRecoDigiInfo(recodigi,_caloRDIs);
        if(_conf.calo().fillDigis()){
          const auto& digi = recodigi.caloDigiPtr();
          for(uint digiIdx = 0; digiIdx < _caloDIs.size(); digiIdx++){
            if(_caloDIs[digiIdx] == *digi){
              _caloDIs[digiIdx].caloRecoDigiIdx_ = _caloRDIs.size()-1;
              _caloRDIs.back().caloDigiIdx_ = digiIdx;
              break;
            }
          }
          if(_caloRDIs.back().caloDigiIdx_ < 0){
            throw cet::exception("EventNtuple") << "Could not find CaloDigi linked to CaloRecoDigi " << _caloRDIs.size()-1 << "\n";
          }
        }
      }
    }
    if(_conf.calo().fill() && _conf.calo().fillHits()){
      event.getByLabel(_conf.calo().hitsTag(),_caloHits);
      for(const auto& hit : *_caloHits.product()){
        _infoStructHelper.fillCaloHitInfo(hit,_caloHIs);
        if(_conf.calo().fillRecoDigis()){
          for(const auto& recodigi : hit.recoCaloDigis()){
            for(uint recoDigiIdx = 0; recoDigiIdx < _caloRDIs.size(); recoDigiIdx++){
              if(_caloRDIs[recoDigiIdx] == *recodigi){
                _caloRDIs[recoDigiIdx].caloHitIdx_ = _caloHIs.size()-1;
                _caloHIs.back().recoDigis_.push_back(recoDigiIdx);
                break;
              }
            }
          }
          if(int(_caloHIs.back().recoDigis_.size()) != _caloHIs.back().nSiPMs_){
            throw cet::exception("EventNtuple") << "Could not find one or all CaloRecoDigi linked to CaloHit " << _caloHIs.size()-1 << "\n";
          }
        }
      }
    }
    if(_conf.calo().fill() && _conf.calo().fillClusters()){
      event.getByLabel(_conf.calo().clustersTag(),_caloClusters);
      for(const auto& cluster : *_caloClusters.product()){
        _infoStructHelper.fillCaloClusterInfo(cluster,_caloCIs);
        if(_conf.calo().fillHits()){
          for(const auto& hit : cluster.caloHitsPtrVector()){
            for(uint hitIdx = 0; hitIdx < _caloHIs.size(); hitIdx++){
              if(_caloHIs[hitIdx] == *hit){
                _caloHIs[hitIdx].clusterIdx_ = _caloCIs.size()-1;
                _caloCIs.back().hits_.push_back(hitIdx);
                break;
              }
            }
          }
          if(_caloCIs.back().hits_.size() != _caloCIs.back().size_){
            throw cet::exception("EventNtuple") << "Could not find one or all CaloHits linked to CaloCluster " << _caloCIs.size()-1 << "\n";
          }
        }
      }
    }

    // ── CRV ───────────────────────────────────────────────────────────────
    if(_conf.crv().fill() && _conf.crv().fillCoincs()){
      _crvcoincs.clear();
      _crvcoincsmc.clear();
      _crvcoincsmcplane.clear();
      event.getByLabel(_conf.crv().coincidencesTag(),_crvCoincidences);
      event.getByLabel(_conf.crv().recoPulsesTag(),_crvRecoPulses);
      event.getByLabel(_conf.crv().stepsTag(),_crvSteps);
      if(fillCRVMC()) event.getByLabel(_conf.crv().mc().coincidenceMCsTag(),_crvCoincidenceMCs);
      _crvHelper.FillCrvHitInfoCollections(
          _crvCoincidences, _crvCoincidenceMCs,
          _crvRecoPulses, _crvSteps, _mcTrajectories, _crvcoincs, _crvcoincsmc,
          _crvsummary, _crvsummarymc, _crvcoincsmcplane, _conf.crv().planeY(), _pph);
    }
    if(_conf.crv().fill() && _conf.crv().fillPulses()){
      _crvpulses.clear();
      _crvpulsesmc.clear();
      event.getByLabel(_conf.crv().recoPulsesTag(),_crvRecoPulses);
      if(fillCRVMC()) event.getByLabel(_conf.crv().mc().digiMCsTag(),_crvDigiMCs);
      _crvHelper.FillCrvPulseInfoCollections(_crvRecoPulses, _crvDigiMCs, _ewmh,
          _crvpulses, _crvpulsesmc);
    }
    if(_conf.crv().fill() && _conf.crv().fillDigis()){
      _crvdigis.clear();
      event.getByLabel(_conf.crv().digisTag(),_crvDigis);
      _crvHelper.FillCrvDigiInfoCollections(_crvDigis,_crvdigis);
    }

    // CRV cosmic inference
    if(_fillCrvInference) {
      _crvInference.clear();
      art::Handle<art::Assns<KalSeed, CrvCoincidenceCluster, MVAResult>> crvInfHandle;
      event.getByLabel(_crvInferenceTag, crvInfHandle);
      std::map<art::Ptr<KalSeed>, std::vector<MVAResultInfo>> crvInfMap;
      if(crvInfHandle.isValid()) {
        for(const auto& assn : *crvInfHandle) {
          MVAResultInfo info;
          info.result = assn.data->_value;
          info.valid = true;
          crvInfMap[assn.first].push_back(info);
        }
      }
      const auto& kseedptr_coll = *_allKSPCHs.at(0);
      for(size_t i = 0; i < kseedptr_coll.size(); ++i) {
        auto it = crvInfMap.find(kseedptr_coll[i]);
        if(it != crvInfMap.end()) {
          _crvInference.push_back(it->second);
        } else {
          _crvInference.emplace_back();
        }
      }
    }

    // global MC step branches (not per-track)
    if(fillEventMC()) {
      for(size_t icoll = 0; icoll < _stepPointMCTags.size(); icoll++){
        auto const& mcsteps = *_stepPointMCCollections[icoll];
        _infoMCStructHelper.fillStepPointMCInfo(mcsteps, _stepPointMCInfos[icoll]);
      }
    }

    // fill this row in the TTree
    bool fill = true;
    if(_hastrks && ntrks == 0) fill = false;
    if(_hascrv && _crvsummary.totalPEs == 0) fill = false;
    if(fill) {
      _ntuple->Fill();
    }
    _hProcEvents->Fill(0);
  }


  void EventNtupleMaker::fillEventInfo( const art::Event& event) {
    _einfo.event  = event.event();
    _einfo.run    = event.run();
    _einfo.subrun = event.subRun();

    if(_recoCountTag != "") {
      art::Handle<mu2e::RecoCount> recoCountHandle;
      if(event.getByLabel(_recoCountTag, recoCountHandle)) {
        _infoStructHelper.fillHitCount(*recoCountHandle, _hcnt);
      }
    }

    if(_PBTTag != "") {
      auto PBThandle = event.getValidHandle<mu2e::ProtonBunchTime>(_PBTTag);
      _einfo.pbtime = PBThandle->pbtime_;
      _einfo.pbterr = PBThandle->pbterr_;
    }

    if(_EWMTag != "") {
      event.getByLabel(_EWMTag, _ewmh);
    }

    if(fillEventMC()) {
      auto PBTMChandle = event.getValidHandle<mu2e::ProtonBunchTimeMC>(_conf.mc().PBTMCTag());
      _einfomc.pbtime = PBTMChandle->pbtime_;

      auto PBIhandle = event.getValidHandle<mu2e::ProtonBunchIntensity>(_PBITag);
      _einfomc.nprotons = PBIhandle->intensity();
    }

    std::vector<Float_t> weights;
    for(const auto& i_weightHandle : _wtHandles) {
      weights.push_back(i_weightHandle->weight());
    }
    _wtinfo.setWeights(weights);
  }

  void EventNtupleMaker::fillTriggerBranch(const art::Event& event,std::string const& process, bool firstEvent) {
    art::InputTag const tag{Form("TriggerResults::%s", process.c_str())};
    auto trigResultsH = event.getValidHandle<art::TriggerResults>(tag);
    const art::TriggerResults* trigResults = trigResultsH.product();
    TriggerResultsNavigator tnav(trigResults);

    if(firstEvent) {
      if(tnav.getTrigPaths().size() > TrigInfo::ntrig_) {
        throw cet::exception("EventNtuple") << "More trigger paths in TriggerResultsNavigator than maximum allowed by TrigInfo::ntrig_. Increase TrigInfo::ntrig_ and rebuild\n";
      }
      for(unsigned int i = 0; i < tnav.getTrigPaths().size(); ++i) {
        const std::string name = "trig_"+tnav.getTrigPathNameByIndex(i);
        _ntuple->Branch(name.c_str(), &_triggerResults._triggerArray[i], _buffsize,_splitlevel);
      }
    }
    for(unsigned int i = 0; i < tnav.getTrigPaths().size(); ++i) {
      _triggerResults._triggerArray[i] = tnav.accepted(tnav.getTrigPathNameByIndex(i));
    }
  }

  void EventNtupleMaker::fillTrackBranches(const art::Handle<KalSeedPtrCollection>& kspch, TrkFitBranchIndex i_trk_fit_branch, size_t i_kseedptr) {

    const auto& kseedptr = (kspch->at(i_kseedptr));
    const auto& kseed = *kseedptr;
    // general info
    _infoStructHelper.fillTrkInfo(kseed,_allTIs.at(i_trk_fit_branch));
    _infoStructHelper.fillTrkSegInfo(kseed,_allTSIs.at(i_trk_fit_branch));
    if(_ftype == LoopHelix    && kseed.loopHelixFit())    _infoStructHelper.fillLoopHelixInfo(kseed,_allLHIs.at(i_trk_fit_branch));
    if(_ftype == CentralHelix && kseed.centralHelixFit()) _infoStructHelper.fillCentralHelixInfo(kseed,_allCHIs.at(i_trk_fit_branch));
    if(_ftype == KinematicLine && kseed.kinematicLineFit()) _infoStructHelper.fillKinematicLineInfo(kseed,_allKLIs.at(i_trk_fit_branch));
    const TrkFitConfig& trkFitConfig = _allTrkFitBranches.at(i_trk_fit_branch);
    if(_conf.diag() > 1 || (_conf.trk().fillHits() && trkFitConfig.options().fillhits())){
      _infoStructHelper.fillHitInfo(kseed, _allTSHIs.at(i_trk_fit_branch), _allTSHCIs.at(i_trk_fit_branch), _conf.trk().fillHitCalibs());
      _infoStructHelper.fillMatInfo(kseed, _allTSMIs.at(i_trk_fit_branch));
    }

    _infoStructHelper.fillTrkCaloHitInfo(kseed, _allTCHIs.at(i_trk_fit_branch));
    if(kseed.hasCaloCluster()) {
      _tcnt._ndec = 1;
      if(_conf.debug()>0){
        auto const& tch = kseed.caloHit();
        auto const& cc = tch.caloCluster();
        std::cout << "CaloCluster has energy " << cc->energyDep()
          << " +- " << cc->energyDepErr() << std::endl;
      }
    }

    const auto& trkQualHandles = _allTrkQualCHs.at(i_trk_fit_branch);
    for(size_t i_trkQualHandle = 0; i_trkQualHandle < trkQualHandles.size(); ++i_trkQualHandle) {
      const auto& trkQualHandle = trkQualHandles.at(i_trkQualHandle);
      if(trkQualHandle.isValid()) {
        _infoStructHelper.fillTrkQualInfo(kseed, trkQualHandle->at(i_kseedptr), _allTrkQualResults.at(i_trk_fit_branch).at(i_trkQualHandle));
      }
    }

    std::vector<Float_t> recoQuals;
    for(const auto& i_recoQualHandle : _allRQCHs.at(i_trk_fit_branch)) {
      recoQuals.push_back(i_recoQualHandle->at(i_kseedptr)._value);
      recoQuals.push_back(i_recoQualHandle->at(i_kseedptr)._calib);
    }
    _allRQIs.at(i_trk_fit_branch).setQuals(recoQuals);

    const auto& trkPIDHandles = _allTrkPIDCHs.at(i_trk_fit_branch);
    for(size_t i_trkPIDHandle = 0; i_trkPIDHandle < trkPIDHandles.size(); ++i_trkPIDHandle) {
      const auto& trkPIDHandle = trkPIDHandles.at(i_trkPIDHandle);
      if(trkPIDHandle.isValid()) {
        _infoStructHelper.fillTrkPIDInfo(kseed, trkPIDHandle->at(i_kseedptr), _allTrkPIDResults.at(i_trk_fit_branch).at(i_trkPIDHandle));
      }
    }

    // fill tracker MC info
    if(fillTrkMC(trkFitConfig.options())) {
      const PrimaryParticle& primary = *_pph;
      if(_conf.debug() > 1) std::cout << "KalSeedMCMatch has " << _ksmcah->size() << " entries" << std::endl;
      for(auto iksmca = _ksmcah->begin(); iksmca != _ksmcah->end(); iksmca++){
        if(_conf.debug() > 2) std::cout << "KalSeed Ptr " << kseedptr << " match Ptr " << iksmca->first << "?" << std::endl;
        if(iksmca->first == kseedptr) {
          auto const& kseedmc = *(iksmca->second);
          _infoMCStructHelper.fillTrkInfoMC(kseed, kseedmc, _surfaceStepsHandle, _allMCTIs.at(i_trk_fit_branch));
          _infoMCStructHelper.fillVDInfo(kseed, kseedmc, _allMCVDInfos.at(i_trk_fit_branch));
          _infoMCStructHelper.fillAllSimInfos(kseedmc, primary, _allMCSimTIs.at(i_trk_fit_branch), trkFitConfig.options().genealogyDepth(), trkFitConfig.options().matchDepth());
          if(_conf.diag() > 1 || (_conf.trk().fillHits() && trkFitConfig.options().fillhits())){
            _infoMCStructHelper.fillHitInfoMCs(kseed,kseedmc, _allTSHIMCs.at(i_trk_fit_branch));
          }
          for(size_t ixt = 0; ixt < _extraMCStepTags.size(); ixt++){
            _infoMCStructHelper.fillExtraMCStepInfos(kseedmc,*_extraMCStepCollections[ixt],
                _extraMCStepInfos[i_trk_fit_branch][ixt],_extraMCStepSummaryInfos.at(i_trk_fit_branch).at(ixt));
          }
          if(_surfaceStepsHandle.isValid()){
            if(_conf.debug() > 2) std::cout << "SurfaceSteps from handle " << _surfaceStepsHandle << std::endl;
            auto& ssi = _surfaceStepInfos.at(i_trk_fit_branch);
            ssi.push_back(std::vector<SurfaceStepInfo>());
            _infoMCStructHelper.fillSurfaceStepInfos(kseedmc,*_surfaceStepsHandle,ssi.back());
          }
          break;
        }
      }
      if(kseed.hasCaloCluster() && fillCaloTrackMatchMC()) {
        auto index = kseed.caloCluster().key();
        auto const& ccmc = (*_ccmcch)[index];
        _infoMCStructHelper.fillCaloClusterInfoMC(ccmc,_allMCTCHIs.at(i_trk_fit_branch));
      }
    }
  }

  // some branches can't be made until the analyze() function because we want to write out all data products of a certain type
  template <typename T, typename TI, typename TIA>
  std::vector<art::Handle<T> >  EventNtupleMaker::createSpecialBranch(const art::Event& event, const std::string& branchname,
  std::vector<art::Handle<T> >& handles,
  TI& infostruct, TIA& array, bool make_individual_branches, const std::string& selection) {
    std::vector<art::Handle<T> > outputHandles;
    std::vector<art::Handle<T> > inputHandles = event.getMany<T>();
    if(inputHandles.size()>0) {
      std::vector<std::string> labels;
      for(const auto& i_handle : inputHandles) {
        std::string moduleLabel = i_handle.provenance()->moduleLabel();
        size_t pos;
        if(selection != "") {
          pos = moduleLabel.find(selection);
          if(pos == std::string::npos) {
            if(_conf.debug() > 3) std::cout << "Selection not found" << std::endl;
            continue;
          }
          else if(pos+selection.length() != moduleLabel.size()) {
            if(_conf.debug() > 3) std::cout << "Selection wasn't at end of moduleLabel" << std::endl;
            continue;
          }
          moduleLabel = moduleLabel.erase(pos, selection.length());
        }
        std::string instanceName = i_handle.provenance()->productInstanceName();
        std::string bname = moduleLabel;
        if(instanceName != "") bname += "_" + instanceName;
        outputHandles.push_back(i_handle);
        labels.push_back(bname);
      }
      if(make_individual_branches) {
        const std::vector<std::string>& leafnames = infostruct.leafnames(labels);
        int n_leaves = leafnames.size();
        for(int i_leaf = 0; i_leaf < n_leaves; ++i_leaf) {
          std::string thisbranchname = (branchname+"."+leafnames.at(i_leaf));
          if(!_ntuple->GetBranch(thisbranchname.c_str())) {
            _ntuple->Branch(thisbranchname.c_str(), &array[i_leaf]);
          }
        }
      }
      else {
        if(!_ntuple->GetBranch((branchname+".").c_str())) {
          _ntuple->Branch((branchname+".").c_str(), &infostruct, infostruct.leafname(labels).c_str());
        }
      }
    }
    return outputHandles;
  }

  void EventNtupleMaker::resetTrackBranches() {
    for(TrkFitBranchIndex i_trk_fit_branch = 0; i_trk_fit_branch < _allTrkFitBranches.size(); ++i_trk_fit_branch) {
      if (!_allTrkFitBranches.at(i_trk_fit_branch).fill()) continue;
      _allRQIs.at(i_trk_fit_branch).reset();
    }
  }
}  // end namespace mu2e

using mu2e::EventNtupleMaker;
DEFINE_ART_MODULE(EventNtupleMaker)
