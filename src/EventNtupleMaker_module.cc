//
// art module to create the EventNtuple
//

// Mu2e includes
#include "Offline/GeneralUtilities/inc/ParameterSetHelpers.hh"
#include "Offline/MCDataProducts/inc/ProtonBunchIntensity.hh"
#include "Offline/MCDataProducts/inc/EventWeight.hh"
#include "Offline/MCDataProducts/inc/KalSeedMC.hh"
#include "Offline/MCDataProducts/inc/CaloClusterMC.hh"
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

// BaBar includes
#include "BTrk/BaBar/BaBar.hh"
#include "BTrk/KalmanTrack/KalRep.hh"
#include "BTrk/ProbTools/ChisqConsistency.hh"
#include "BTrk/BbrGeom/BbrVectorErr.hh"
#include "BTrk/TrkBase/TrkHelixUtils.hh"
#include "Offline/Mu2eUtilities/inc/TriggerResultsNavigator.hh"
// mu2e tracking
#include "Offline/BTrkData/inc/TrkStrawHit.hh"
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
#include "EventNtuple/inc/HelixInfo.hh"
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

// C++ includes.
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

namespace mu2e {
  // Need this for the BaBar headers.
  using CLHEP::Hep3Vector;
  typedef KalSeedCollection::const_iterator KSCIter;
  typedef size_t BranchIndex;
  typedef size_t StepCollIndex;

  class EventNtupleMaker : public art::EDAnalyzer {

    public:

      struct BranchOptConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        fhicl::Atom<bool> fillmc{Name("fillMC"), Comment("Switch to turn on filling of MC information for this set of tracks"), false};
        fhicl::Atom<bool> fillhits{Name("fillHits"), Comment("Switch to turn on filling of hit-level information for this set of tracks"), false};
        fhicl::Atom<int> genealogyDepth{Name("genealogyDepth"), Comment("The depth of the genealogy information you want to keep"), 1};
        fhicl::Atom<int> matchDepth{Name("matchDepth"), Comment("The depth into the MC true particle matching you want to keep"), 1};
      };

      struct BranchConfig {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;

        fhicl::Atom<std::string> input{Name("input"), Comment("KalSeedCollection input tag")};
        fhicl::Atom<std::string> branch{Name("branch"), Comment("Name of output branch")};
        fhicl::Sequence<std::string> trkQualTags{Name("trkQualTags"), Comment("Input tags for MVAResultCollection to use for TrkQuals")};
        fhicl::Sequence<std::string> trkPIDTags{Name("trkPIDTags"), Comment("Input tags for MVAResultCollection to use for TrkPID")};
        fhicl::Table<BranchOptConfig> options{Name("options"), Comment("Optional arguments for a branch")};
      };

      struct Config {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;

        // General control and config
        fhicl::Atom<int> diag{Name("diagLevel"),1};
        fhicl::Atom<int> debug{Name("debugLevel"),0};
        fhicl::Atom<int> splitlevel{Name("splitlevel"),99};
        fhicl::Atom<int> buffsize{Name("buffsize"),32000};
        fhicl::Atom<bool> hastrks{Name("hasTracks"), Comment("Require >=1 tracks to fill tuple"), false};
        fhicl::Atom<bool> hascrv{Name("hasCRV"), Comment("Require CRV information to fill tuple"), false};
       // General event info
        fhicl::Atom<art::InputTag> rctag{Name("RecoCountTag"), Comment("RecoCount"), art::InputTag()};
        fhicl::Atom<art::InputTag> PBITag{Name("PBITag"), Comment("Tag for ProtonBunchIntensity object") ,art::InputTag()};
        fhicl::Atom<art::InputTag> PBTTag{Name("PBTTag"), Comment("Tag for ProtonBunchTime object") ,art::InputTag()};
        fhicl::Atom<art::InputTag> EWMTag{Name("EWMTag"), Comment("Tag for EventWindowMarker object") ,art::InputTag()};
        fhicl::Atom<bool> filltrig{Name("FillTriggerInfo"),false};
        fhicl::Atom<std::string> trigProcessName{Name("TriggerProcessName"), Comment("Process name for Trigger")};
        fhicl::Atom<std::string> trigpathsuffix{Name("TriggerPathSuffix"), "_trigger"}; // all trigger paths have this in the name
        // core tracking
        fhicl::Sequence<fhicl::Table<BranchConfig> > branches{Name("branches"), Comment("All the branches we want to write")};
        // Additional (optional) tracking information
        fhicl::Atom<bool> fillhits{Name("FillHitInfo"),Comment("Global switch to turn on/off hit-level info"), false};
        fhicl::Atom<bool> fillhitcalibs{Name("FillHitCalibInfo"), Comment("Switch to turn on filling of hit-level information for this set of tracks"), false};
        fhicl::Atom<std::string> fittype{Name("FitType"),Comment("Type of track Fit: LoopHelix, CentralHelix, KinematicLine, or Unknown"),"Unknown"};
        fhicl::Atom<bool> helices{Name("FillHelixInfo"),false};
        // Calorimeter input
        fhicl::Atom<art::InputTag> caloClustersTag{Name("CaloClustersTag"), Comment("Tag for Calorimeter cluster collection"), art::InputTag()};
        fhicl::Atom<art::InputTag> caloHitsTag{Name("CaloHitsTag"), Comment("Tag for Calorimeter hit collection"), art::InputTag()};
        fhicl::Atom<art::InputTag> caloRecoDigisTag{Name("CaloRecoDigisTag"), Comment("Tag for Calorimeter recodigi collection"), art::InputTag()};
        fhicl::Atom<art::InputTag> caloDigisTag{Name("CaloDigisTag"), Comment("Tag for Calorimeter digi collection"), art::InputTag()};
        // Calorimeter flags
        fhicl::Atom<bool> fillCaloClusters{Name("FillCaloClusters"),Comment("Flag for turning on Calo Clusters branch"), true};
        fhicl::Atom<bool> fillCaloHits{Name("FillCaloHits"),Comment("Flag for turning on Calo Hits branch"), false};
        fhicl::Atom<bool> fillCaloRecoDigis{Name("FillCaloRecoDigis"),Comment("Flag for turning on Calo RecoDigis branch"), false};
        fhicl::Atom<bool> fillCaloDigis{Name("FillCaloDigis"),Comment("Flag for turning on Calo Digis branch"), false};
        // CRV -- input tags
        fhicl::Atom<art::InputTag> crvCoincidencesTag{Name("CrvCoincidencesTag"), Comment("Tag for CrvCoincidenceCluster Collection"), art::InputTag()};
        fhicl::Atom<art::InputTag> crvRecoPulsesTag{Name("CrvRecoPulsesTag"), Comment("Tag for CrvRecopPulse Collection"), art::InputTag()};
        fhicl::Atom<art::InputTag> crvStepsTag{Name("CrvStepsTag"), Comment("Tag for CrvStep Collection"), art::InputTag()};
        fhicl::Atom<art::InputTag> crvDigiMCsTag{Name("CrvDigiMCsTag"), Comment("Tag for CrvDigiMC Collection"), art::InputTag()};
        fhicl::Atom<art::InputTag> crvDigisTag{Name("CrvDigisTag"), Comment("Tag for CrvDigi Collection"), art::InputTag()};
        // CRV -- flags
        fhicl::Atom<bool> fillcrvcoincs{Name("FillCRVCoincs"),Comment("Flag for turning on crv CoincidenceClusterbranches"), false};
        fhicl::Atom<bool> fillcrvpulses{Name("FillCRVPulses"),Comment("Flag for turning on crvpulses(mc) branches"), false};
        fhicl::Atom<bool> fillcrvdigis{Name("FillCRVDigis"),Comment("Flag for turning on crvdigis branch"), false};
        // CRV -- other
        fhicl::Atom<double> crvPlaneY{Name("CrvPlaneY"),2751.485};  //y of center of the top layer of the CRV-T counters.  This belongs in KinKalGeom as an intersection plane, together with the rest of the CRV planes FIXME
        // MC truth
        fhicl::Atom<bool> fillmc{Name("FillMCInfo"),Comment("Global switch to turn on/off MC info"),true};
        fhicl::Table<InfoMCStructHelper::Config> infoMCStructHelper{Name("InfoMCStructHelper"), Comment("Configuration for the InfoMCStructHelper")};
        fhicl::Atom<art::InputTag> PBTMCTag{Name("PBTMCTag"), Comment("Tag for ProtonBunchTimeMC object") ,art::InputTag()};
        fhicl::Atom<art::InputTag> simParticlesTag{Name("SimParticlesTag"), Comment("SimParticle Collection Tag")};
        fhicl::Atom<art::InputTag> mcTrajectoriesTag{Name("MCTrajectoriesTag"), Comment("MCTrajectory Collection Tag")};
        fhicl::Atom<art::InputTag> primaryParticleTag{Name("PrimaryParticleTag"), Comment("Tag for PrimaryParticle"), art::InputTag()};
        fhicl::Atom<art::InputTag> kalSeedMCTag{Name("KalSeedMCAssns"), Comment("Tag for KalSeedMCAssn"), art::InputTag()};
        // extra MC
        fhicl::OptionalSequence<art::InputTag> extraMCStepTags{Name("ExtraMCStepCollectionTags"), Comment("Input tags for any other StepPointMCCollections you want written out")};
        // passive elements and Virtual Detector MC information
        fhicl::OptionalAtom<art::InputTag> SurfaceStepsTag{Name("SurfaceStepCollectionTag"), Comment("SurfaceStep Collection")};
        // Calo MC
        fhicl::Atom<bool> fillCaloMC{ Name("FillCaloMC"),Comment("Fill CaloMC information"), true};
        fhicl::Atom<art::InputTag> caloClusterMCTag{Name("CaloClusterMCTag"), Comment("Tag for CaloClusterMCCollection") ,art::InputTag()};
        // CRV MC
        fhicl::Atom<art::InputTag> crvCoincidenceMCsTag{Name("CrvCoincidenceMCsTag"), Comment("Tag for CrvCoincidenceClusterMC Collection"), art::InputTag()};
        fhicl::Atom<art::InputTag> crvMCAssnsTag{ Name("CrvCoincidenceClusterMCAssnsTag"), Comment("art::InputTag for CrvCoincidenceClusterMCAssns")};
        // Pre-processed analysis info; are these redundant with the branch config ?
        fhicl::Atom<bool> filltrkpid{Name("FillTrkPIDInfo"),false};
        fhicl::Atom<bool> filltrkqual{Name("FillTrkQual"),false};
      };
      typedef art::EDAnalyzer::Table<Config> Parameters;

      explicit EventNtupleMaker(const Parameters& conf);
      virtual ~EventNtupleMaker() { }

      void beginJob() override;
      void beginSubRun(const art::SubRun & subrun ) override;
      void analyze(const art::Event& e) override;

    private:

      Config _conf;
      std::vector<BranchConfig> _allBranches; // configurations for all track branches
      // main TTree
      TTree* _ntuple;
      TH1I* _hVersion;
      // general event info branch
      EventInfo _einfo;
      EventInfoMC _einfomc;
      art::InputTag _recoCountTag, _PBITag, _PBTTag, _EWMTag, _PBTMCTag;
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
      std::map<BranchIndex, std::vector<TrkInfo>> _allTIs;
      std::map<BranchIndex, std::vector<std::vector<TrkSegInfo>>> _allTSIs;
      std::map<BranchIndex, std::vector<std::vector<LoopHelixInfo>>> _allLHIs;
      std::map<BranchIndex, std::vector<std::vector<CentralHelixInfo>>> _allCHIs;
      std::map<BranchIndex, std::vector<std::vector<KinematicLineInfo>>> _allKLIs;

      std::map<BranchIndex, std::vector<TrkCaloHitInfo>> _allTCHIs;
      // quality branches (inputs)

      std::vector<std::vector<art::Handle<RecoQualCollection> > > _allRQCHs; // outer vector is for each track type, inner vector is all RecoQuals
      std::vector<std::vector<art::Handle<MVAResultCollection> >> _allTrkQualCHs;
      std::vector<std::vector<art::Handle<MVAResultCollection> >> _allTrkPIDCHs;

      // quality branches (outputs)
      std::vector<RecoQualInfo> _allRQIs;
      std::map<BranchIndex, std::vector<std::vector<MVAResultInfo>>> _allTrkQualResults;
      std::map<BranchIndex, std::vector<std::vector<MVAResultInfo>>> _allTrkPIDResults;

      // trigger information
      unsigned _trigbits;
      std::map<size_t,unsigned> _tmap; // map between path and trigger ID.  ID should come from trigger itself FIXME!
      // MC truth (fcl parameters)
      bool _fillmc;
      // MC truth inputs
      std::vector<art::InputTag> _extraMCStepTags;
      std::vector<art::Handle<StepPointMCCollection>> _extraMCStepCollections;
      std::map<BranchIndex, std::map<StepCollIndex, std::vector<MCStepInfos>>> _extraMCStepInfos;
      std::map<BranchIndex, std::map<StepCollIndex, std::vector<MCStepSummaryInfo>>> _extraMCStepSummaryInfos;
      // SurfaceSteps
      art::InputTag _surfaceStepsTag;
      std::map<BranchIndex, std::vector<std::vector<SurfaceStepInfo>>> _surfaceStepInfos;
      art::Handle<SurfaceStepCollection> _surfaceStepsHandle;
      //
      art::Handle<PrimaryParticle> _pph;
      art::Handle<KalSeedMCAssns> _ksmcah;
      art::Handle<SimParticleCollection> _simParticles;
      art::Handle<MCTrajectoryCollection> _mcTrajectories;
      // MC truth branches (outputs)
      std::map<BranchIndex, std::vector<TrkInfoMC>> _allMCTIs;
      std::map<BranchIndex, std::vector<std::vector<SimInfo>>> _allMCSimTIs;
      std::map<BranchIndex, std::vector<std::vector<MCStepInfo>>> _allMCVDInfos;
      bool _fillcalomc;
      art::Handle<CaloClusterMCCollection> _ccmcch;
      std::map<BranchIndex, std::vector<CaloClusterInfoMC>> _allMCTCHIs;

      // hit level info branches
      std::map<BranchIndex, std::vector<std::vector<TrkStrawHitInfo>>> _allTSHIs;
      std::map<BranchIndex, std::vector<std::vector<TrkStrawHitCalibInfo>>> _allTSHCIs;
      std::map<BranchIndex, std::vector<std::vector<TrkStrawMatInfo>>> _allTSMIs;
      std::map<BranchIndex, std::vector<std::vector<TrkStrawHitInfoMC>>> _allTSHIMCs;

      // event weights
      std::vector<art::Handle<EventWeight> > _wtHandles;
      EventWeightInfo _wtinfo;

      // Calorimeter
      art::Handle<CaloClusterCollection> _caloClusters;
      art::Handle<CaloHitCollection> _caloHits;
      art::Handle<CaloRecoDigiCollection> _caloRecoDigis;
      art::Handle<CaloDigiCollection> _caloDigis;
      std::vector<CaloClusterInfo> _caloCIs;
      TrigInfo _triggerResults;
      std::vector<CaloHitInfo> _caloHIs;
      std::vector<CaloRecoDigiInfo> _caloRDIs;
      std::vector<CaloDigiInfo> _caloDIs;
      bool _fillcaloclusters, _fillcalohits, _fillcalorecodigis, _fillcalodigis;

      // CRV (inputs)
      std::map<BranchIndex, std::vector<art::Handle<BestCrvAssns>>> _allBestCrvAssns;
      art::Handle<CrvCoincidenceClusterMCAssns>      _crvMCAssns;
      art::Handle<CrvCoincidenceClusterCollection>   _crvCoincidences;
      art::Handle<CrvCoincidenceClusterMCCollection> _crvCoincidenceMCs;
      art::Handle<CrvRecoPulseCollection>            _crvRecoPulses;
      art::Handle<CrvDigiMCCollection>               _crvDigiMCs;
      art::Handle<CrvDigiCollection>                 _crvDigis;
      art::Handle<CrvStepCollection>                 _crvSteps;
      // CRV -- fhicl parameters
      bool _fillcrvcoincs, _fillcrvpulses, _fillcrvdigis;
      double _crvPlaneY;  // needs to move to KinKalGeom FIXME
      // CRV (output)
      std::vector<CrvHitInfoReco> _crvcoincs;
      std::map<BranchIndex, std::vector<CrvHitInfoReco>> _allBestCrvs; // there can be more than one of these per track type
      std::vector<CrvHitInfoMC> _crvcoincsmc;
      std::map<BranchIndex, std::vector<CrvHitInfoMC>> _allBestCrvMCs;
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
      // helper functions
      void fillEventInfo(const art::Event& event);
      void fillTriggerBranch(const art::Event& event,std::string const& process, bool firstEvent);
      void resetTrackBranches();
      void fillTrackBranches(const art::Handle<KalSeedPtrCollection>& kspch, BranchIndex i_branch, size_t i_kseedptr);

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
    _PBTMCTag(conf().PBTMCTag()),
    _hastrks(conf().hastrks()),
    _hascrv(conf().hascrv()),
    _fillmc(conf().fillmc()),
   _fillcalomc(conf().fillCaloMC()),
    // Calorimeter
    _fillcaloclusters(conf().fillCaloClusters()),
    _fillcalohits(conf().fillCaloHits()),
    _fillcalorecodigis(conf().fillCaloRecoDigis()),
    _fillcalodigis(conf().fillCaloDigis()),
    // CRV
    _fillcrvcoincs(conf().fillcrvcoincs()),
    _fillcrvpulses(conf().fillcrvpulses()),
    _fillcrvdigis(conf().fillcrvdigis()),
    _crvPlaneY(conf().crvPlaneY()),
    _infoMCStructHelper(conf().infoMCStructHelper()),
    _buffsize(conf().buffsize()),
    _splitlevel(conf().splitlevel())
  {

    // decode fit type
    for(size_t ifit=0;ifit < fitNames.size();++ifit){
      auto const& fname = fitNames[ifit];
      if(_conf.fittype() == fname){
        _ftype= (FType)ifit;
        break;
      }
    }

    // Put all the branch configurations together
    for(const auto& branch_cfg : _conf.branches()){
      _allBranches.push_back(branch_cfg);
    }

    // Create all the info structs
    for (BranchIndex i_branch = 0; i_branch < _allBranches.size(); ++i_branch) {
      auto i_branchConfig = _allBranches.at(i_branch);
      _allTIs[i_branch] = std::vector<TrkInfo>();
      // fit sampling (KalIntersection) at a surface
      _allTSIs[i_branch] = std::vector<std::vector<TrkSegInfo>>();
      // fit-specific branches
      _allLHIs[i_branch] = std::vector<std::vector<LoopHelixInfo>>();
      _allCHIs[i_branch] = std::vector<std::vector<CentralHelixInfo>>();
      _allKLIs[i_branch] = std::vector<std::vector<KinematicLineInfo>>();

      // mc truth info at VDs
      _allMCVDInfos[i_branch] = std::vector<std::vector<MCStepInfo>>();

      _allTCHIs[i_branch] = std::vector<TrkCaloHitInfo>();

      _allMCTIs[i_branch] = std::vector<TrkInfoMC>();
      _allMCSimTIs[i_branch] = std::vector<std::vector<SimInfo>>();

      if(_fillcalomc){
        _allMCTCHIs[i_branch] = std::vector<CaloClusterInfoMC>();
      }

      RecoQualInfo rqi;
      _allRQIs.push_back(rqi);

      _allTSHIs[i_branch] = std::vector<std::vector<TrkStrawHitInfo>>();
      _allTSHCIs[i_branch] = std::vector<std::vector<TrkStrawHitCalibInfo>>();
      _allTSMIs[i_branch] = std::vector<std::vector<TrkStrawMatInfo>>();
      _allTSHIMCs[i_branch] = std::vector<std::vector<TrkStrawHitInfoMC>>();

      std::vector<std::vector<MVAResultInfo>> trkQualResults;
      for (size_t i_trkQualTag = 0; i_trkQualTag < i_branchConfig.trkQualTags().size(); ++i_trkQualTag) {
        trkQualResults.emplace_back(std::vector<MVAResultInfo>());
      }
      _allTrkQualResults[i_branch] = trkQualResults;

      std::vector<std::vector<MVAResultInfo>> trkPIDResults;
      for (size_t i_trkPIDTag = 0; i_trkPIDTag < i_branchConfig.trkPIDTags().size(); ++i_trkPIDTag) {
        trkPIDResults.emplace_back(std::vector<MVAResultInfo>());
      }
      _allTrkPIDResults[i_branch] = trkPIDResults;


      if(_conf.extraMCStepTags(_extraMCStepTags)){
        for (BranchIndex i_branch = 0; i_branch < _allBranches.size(); ++i_branch) {
          for (StepCollIndex i_extraMCStepTag = 0; i_extraMCStepTag < _extraMCStepTags.size(); ++i_extraMCStepTag) {
            _extraMCStepInfos[i_branch][i_extraMCStepTag] = std::vector<MCStepInfos>();
            _extraMCStepSummaryInfos[i_branch][i_extraMCStepTag] = std::vector<MCStepSummaryInfo>();
          }
        }
      }
      if(_conf.SurfaceStepsTag(_surfaceStepsTag)){
        for (BranchIndex i_branch = 0; i_branch < _allBranches.size(); ++i_branch) {
          _surfaceStepInfos[i_branch] = std::vector<std::vector<SurfaceStepInfo>>();
        }
      }
    }
  }

  void EventNtupleMaker::beginJob( ){
    art::ServiceHandle<art::TFileService> tfs;
    // create TTree
    _ntuple=tfs->make<TTree>("ntuple","Mu2e Event Ntuple");
    _hVersion = tfs->make<TH1I>("version", "version number",3,0,3);
    _hVersion->GetXaxis()->SetBinLabel(1, "major"); _hVersion->SetBinContent(1, 6);
    _hVersion->GetXaxis()->SetBinLabel(2, "minor"); _hVersion->SetBinContent(2, 8);
    _hVersion->GetXaxis()->SetBinLabel(3, "patch"); _hVersion->SetBinContent(3, 1);
    // add event info branch
    _ntuple->Branch("evtinfo",&_einfo,_buffsize,_splitlevel);
    if (_fillmc) {
      _ntuple->Branch("evtinfomc",&_einfomc,_buffsize,_splitlevel);
    }
    // hit counting branch
    _ntuple->Branch("hitcount",&_hcnt);
    // track counting branch
    _ntuple->Branch("tcnt",&_tcnt);

    // create all track branches
    for (BranchIndex i_branch = 0; i_branch < _allBranches.size(); ++i_branch) {
      BranchConfig i_branchConfig = _allBranches.at(i_branch);
      std::string branch = i_branchConfig.branch();
      _ntuple->Branch((branch+".").c_str(),&_allTIs.at(i_branch),_buffsize,_splitlevel);
      _ntuple->Branch((branch+"segs.").c_str(),&_allTSIs.at(i_branch),_buffsize,_splitlevel);
// add traj-specific branches
      if(_ftype == LoopHelix )_ntuple->Branch((branch+"segpars_lh.").c_str(),&_allLHIs.at(i_branch),_buffsize,_splitlevel);
      if(_ftype == CentralHelix )_ntuple->Branch((branch+"segpars_ch.").c_str(),&_allCHIs.at(i_branch),_buffsize,_splitlevel);
      if(_ftype == KinematicLine )_ntuple->Branch((branch+"segpars_kl.").c_str(),&_allKLIs.at(i_branch),_buffsize,_splitlevel);
      // TrkCaloHit: currently only 1
      _ntuple->Branch((branch+"calohit.").c_str(),&_allTCHIs.at(i_branch));
      for (size_t i_trkQualTag = 0; i_trkQualTag < i_branchConfig.trkQualTags().size(); ++i_trkQualTag) {
        std::string branchname = "qual";
        if (i_trkQualTag > 0) {
          branchname += std::to_string(i_trkQualTag+1); // +1 so that the second trkqual will be "trkqual2"
        }
        _ntuple->Branch((branch+branchname+".").c_str(),&_allTrkQualResults.at(i_branch).at(i_trkQualTag),_buffsize,_splitlevel);
      }
      for (size_t i_trkPIDTag = 0; i_trkPIDTag < i_branchConfig.trkPIDTags().size(); ++i_trkPIDTag) {
        std::string branchname = "pid";
        if (i_trkPIDTag > 0) {
          branchname += std::to_string(i_trkPIDTag+1); // +1 so that the second trkpid will be "trkpid2"
        }
        _ntuple->Branch((branch+branchname+'.').c_str(),&_allTrkPIDResults.at(i_branch).at(i_trkPIDTag),_buffsize,_splitlevel);
      }
      // optionally add hit-level branches
      // (for the time being diagLevel : 2 will still work, but I propose removing this at some point)
      if(_conf.diag() > 1 || (_conf.fillhits() && i_branchConfig.options().fillhits())){
        _ntuple->Branch((branch+"hits.").c_str(),&_allTSHIs.at(i_branch),_buffsize,_splitlevel);
        if (_conf.fillhitcalibs())
          _ntuple->Branch((branch+"hitcalibs.").c_str(),&_allTSHCIs.at(i_branch),_buffsize,_splitlevel);
        _ntuple->Branch((branch+"mats.").c_str(),&_allTSMIs.at(i_branch),_buffsize,_splitlevel);
      }

      // optionally add MC branches
      if(_fillmc && i_branchConfig.options().fillmc()){
        _ntuple->Branch((branch+"mc.").c_str(),&_allMCTIs.at(i_branch),_buffsize,_splitlevel);

        _ntuple->Branch((branch+"mcsim.").c_str(),&_allMCSimTIs.at(i_branch),_buffsize,_splitlevel);
        _ntuple->Branch((branch+"mcvd.").c_str(),&_allMCVDInfos.at(i_branch),_buffsize,_splitlevel);
        if(_fillcalomc)_ntuple->Branch((branch+"calohitmc.").c_str(),&_allMCTCHIs.at(i_branch),_buffsize,_splitlevel);
        // at hit-level MC information
        // (for the time being diagLevel will still work, but I propose removing this at some point)
        if(_conf.diag() > 1 || (_conf.fillhits() && i_branchConfig.options().fillhits())){
          _ntuple->Branch((branch+"hitsmc.").c_str(),&_allTSHIMCs.at(i_branch),_buffsize,_splitlevel);
        }
        // configure extra MCStep branches for this track type
        if(_conf.extraMCStepTags(_extraMCStepTags)){
          for(size_t ixtra=0;ixtra < _extraMCStepTags.size(); ++ixtra) {
            auto const& tag = _extraMCStepTags[ixtra];
            auto inst = tag.instance();
            std::string  mcsiname = branch +"mcsic_" + inst + ".";
            std::string  mcssiname = branch + "mcssi_" + inst + ".";
            _ntuple->Branch(mcsiname.c_str(),&_extraMCStepInfos[i_branch][ixtra],_buffsize,_splitlevel);
            _ntuple->Branch(mcssiname.c_str(),&_extraMCStepSummaryInfos[i_branch][ixtra],_buffsize,_splitlevel);
          }
        }
        if(_conf.SurfaceStepsTag(_surfaceStepsTag)){
          _ntuple->Branch((branch+"segsmc.").c_str(),&_surfaceStepInfos[i_branch],_buffsize,_splitlevel);
        }
      }
    }

    // Calorimeter
    if (_fillcaloclusters){
      _ntuple->Branch("caloclusters.",&_caloCIs,_buffsize,_splitlevel);
    }
    if (_fillcalohits){
      _ntuple->Branch("calohits.",&_caloHIs,_buffsize,_splitlevel);
    }
    if (_fillcalorecodigis){
      _ntuple->Branch("calorecodigis.",&_caloRDIs,_buffsize,_splitlevel);
    }
    if (_fillcalodigis){
      _ntuple->Branch("calodigis.",&_caloDIs,_buffsize,_splitlevel);
    }

    // general CRV info
    if(_fillcrvcoincs) {
      // coincidence branches should be here FIXME
      _ntuple->Branch("crvsummary",&_crvsummary,_buffsize,_splitlevel);
      _ntuple->Branch("crvcoincs.",&_crvcoincs,_buffsize,_splitlevel);
      if(_fillcrvpulses) {
        _ntuple->Branch("crvpulses.",&_crvpulses,_buffsize,_splitlevel);
      }
      if(_fillcrvdigis) {
        _ntuple->Branch("crvdigis.",&_crvdigis,_buffsize,_splitlevel);
      }
      if(_fillmc){
        _ntuple->Branch("crvsummarymc",&_crvsummarymc,_buffsize,_splitlevel);
        _ntuple->Branch("crvcoincsmc.",&_crvcoincsmc,_buffsize,_splitlevel);
        _ntuple->Branch("crvcoincsmcplane.",&_crvcoincsmcplane,_buffsize,_splitlevel);
        if(_fillcrvpulses) {
          _ntuple->Branch("crvpulsesmc.",&_crvpulsesmc,_buffsize,_splitlevel);
        }
      }
    }
    // helix info
    if(_conf.helices()) _ntuple->Branch("helices.",&_hinfos,_buffsize,_splitlevel);
  }

  void EventNtupleMaker::beginSubRun(const art::SubRun & subrun ) {
    // get bfield
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
    if(_fillmc){
      _infoMCStructHelper.updateEvent(event);
    }
    // fill event level info
    fillEventInfo(event);

    // need to create and define the event weight branch here because we only now know the EventWeight creating modules that have been run through the Event
    std::vector<art::Handle<EventWeight> > eventWeightHandles;
    _wtHandles = createSpecialBranch(event, "evtwt", eventWeightHandles, _wtinfo, _wtinfo._weights, false);

    std::string process = _conf.trigProcessName();
    _allKSPCHs.clear();
    _allRQCHs.clear();
    _allBestCrvAssns.clear();
    _allTrkQualCHs.clear();
    _allTrkPIDCHs.clear();

    art::Handle<KalHelixAssns> khaH;
    if(_conf.helices()){ // find associated Helices
      BranchConfig i_branchConfig = _allBranches.at(0);
      art::InputTag kalSeedInputTag = i_branchConfig.input();
      event.getByLabel(kalSeedInputTag,khaH);
    }

    for (BranchIndex i_branch = 0; i_branch < _allBranches.size(); ++i_branch) {
      BranchConfig i_branchConfig = _allBranches.at(i_branch);
      art::Handle<KalSeedPtrCollection> kalSeedPtrCollHandle;
      art::InputTag kalSeedPtrInputTag = i_branchConfig.input();
      event.getByLabel(kalSeedPtrInputTag,kalSeedPtrCollHandle);
      _allKSPCHs.push_back(kalSeedPtrCollHandle);

      std::vector<art::Handle<MVAResultCollection>> trkQualCollHandles;
      for (const auto& i_trkQualTag : i_branchConfig.trkQualTags()) {
        art::Handle<MVAResultCollection> trkQualCollHandle;
        event.getByLabel(i_trkQualTag,trkQualCollHandle);
        trkQualCollHandles.push_back(trkQualCollHandle);
      }
      _allTrkQualCHs.emplace_back(trkQualCollHandles);

      // also create the reco qual branches
      std::vector<art::Handle<RecoQualCollection> > recoQualCollHandles;
      std::vector<art::Handle<RecoQualCollection> > selectedRQCHs;
      selectedRQCHs = createSpecialBranch(event, i_branchConfig.branch()+"qual", recoQualCollHandles, _allRQIs.at(i_branch), _allRQIs.at(i_branch)._qualsAndCalibs, true);
      for (const auto& i_selectedRQCH : selectedRQCHs) {
        if (i_selectedRQCH->size() != kalSeedPtrCollHandle->size()) {
          throw cet::exception("TrkAna") << "Sizes of KalSeedPtrCollection and this RecoQualCollection are inconsistent (" << kalSeedPtrCollHandle->size() << " and " << i_selectedRQCH->size() << " respectively)";
        }
      }
      _allRQCHs.push_back(selectedRQCHs);

      // TrkCaloHitPID
      std::vector<art::Handle<MVAResultCollection>> trkPIDCollHandles;
      for (const auto& i_trkPIDTag : i_branchConfig.trkPIDTags()) {
        art::Handle<MVAResultCollection> trkPIDCollHandle;
        event.getByLabel(i_trkPIDTag,trkPIDCollHandle);
        trkPIDCollHandles.push_back(trkPIDCollHandle);
      }
      _allTrkPIDCHs.emplace_back(trkPIDCollHandles);

    }

    // trigger information
    if(_conf.filltrig()){
      fillTriggerBranch(event, process, firstEvent);
      firstEvent=false;
    }
    
    // MC data
    if(_fillmc) { // get MC product collections
      event.getByLabel(_conf.primaryParticleTag(),_pph);
      event.getByLabel(_conf.kalSeedMCTag(),_ksmcah);
      event.getByLabel(_conf.simParticlesTag(),_simParticles);
      event.getByLabel(_conf.mcTrajectoriesTag(),_mcTrajectories);
      if(_fillcalomc)event.getByLabel(_conf.caloClusterMCTag(),_ccmcch);
    }

    //
    // fill track counts: count unique PDG hypotheses
    //
    std::map<int,int> pdgCounts;
    for (BranchIndex i_branch = 0; i_branch < _allBranches.size() && i_branch < _allKSPCHs.size(); ++i_branch) {
      const auto& kseedptr_coll_h = _allKSPCHs.at(i_branch);
      const auto& kseedptr_coll = *kseedptr_coll_h;
      for (size_t i_kseedptr = 0; i_kseedptr < kseedptr_coll.size(); ++i_kseedptr) {
        const auto& kseedptr = kseedptr_coll[i_kseedptr];
        pdgCounts[kseedptr->particle()]++;
      }
    }
    // the number of tracks is the max count of any PDG hypothesis
    _tcnt.ntrk = 0;
    for (const auto& pair : pdgCounts) {
      _tcnt.ntrk = std::max(_tcnt.ntrk, pair.second);
    }

    // find extra MCStep collections
    _extraMCStepCollections.clear();
    for(size_t ixt = 0; ixt < _extraMCStepTags.size(); ixt++){
      auto const& tag = _extraMCStepTags[ixt];
      art::Handle<StepPointMCCollection> mcstepch;
      event.getByLabel(tag,mcstepch);
      _extraMCStepCollections.push_back(mcstepch);
    }
    event.getByLabel(_surfaceStepsTag,_surfaceStepsHandle);

    // loop through all track types
    unsigned ntrks(0);
    for (BranchIndex i_branch = 0; i_branch < _allBranches.size(); ++i_branch) {
      BranchConfig i_branchConfig = _allBranches.at(i_branch);

      _allTIs.at(i_branch).clear();
      _allTSIs.at(i_branch).clear();
      _allLHIs.at(i_branch).clear();
      _allCHIs.at(i_branch).clear();
      _allKLIs.at(i_branch).clear();
      _allTCHIs.at(i_branch).clear();

      _allTSHIs.at(i_branch).clear();
      _allTSHCIs.at(i_branch).clear();
      _allTSMIs.at(i_branch).clear();
      _allTSHIMCs.at(i_branch).clear();

      _allMCTIs.at(i_branch).clear();
      _allMCVDInfos.at(i_branch).clear();
      _allMCSimTIs.at(i_branch).clear();

      for (size_t i_trkQualTag = 0; i_trkQualTag < i_branchConfig.trkQualTags().size(); ++i_trkQualTag) {
        _allTrkQualResults.at(i_branch).at(i_trkQualTag).clear();
      }

      for (size_t i_trkPIDTag = 0; i_trkPIDTag < i_branchConfig.trkPIDTags().size(); ++i_trkPIDTag) {
        _allTrkPIDResults.at(i_branch).at(i_trkPIDTag).clear();
      }

      for (StepCollIndex i_extraMCStepTag = 0; i_extraMCStepTag < _extraMCStepTags.size(); ++i_extraMCStepTag) {
        _extraMCStepInfos.at(i_branch).at(i_extraMCStepTag).clear();
        _extraMCStepSummaryInfos.at(i_branch).at(i_extraMCStepTag).clear();
      }
      _surfaceStepInfos.at(i_branch).clear();

      if(_fillcalomc) { _allMCTCHIs.at(i_branch).clear(); }

      const auto& kseedptr_coll_h = _allKSPCHs.at(i_branch);
      const auto& kseedptr_coll = *kseedptr_coll_h;
      for (size_t i_kseedptr = 0; i_kseedptr < kseedptr_coll.size(); ++i_kseedptr) {
        resetTrackBranches(); // reset track branches here so that we don't get information from previous tracks in the next entry

        fillTrackBranches(kseedptr_coll_h, i_branch, i_kseedptr); // fill the info structs for this track
        if(_conf.helices()){
          auto const& khassns = khaH.product();
          // find the associated HelixSeed to this KalSeed using the assns.
          auto hptr = (*khassns)[i_kseedptr].second;
          _infoStructHelper.fillHelixInfo(hptr, _hinfos);
        }
        ntrks++; // count total # of tracks
      }
    }

    // Calorimeter
    // Clear lists for this event
    _caloCIs.clear();
    _caloHIs.clear();
    _caloRDIs.clear();
    _caloDIs.clear();
    if(_fillcaloclusters){

      //Get the clusters
      event.getByLabel(_conf.caloClustersTag(),_caloClusters);
      for (const auto& cluster : *_caloClusters.product()){

        int cluster_idx = _caloCIs.size();
        _infoStructHelper.fillCaloClusterInfo(cluster,_caloCIs);

        if (_fillcalohits){
          for (const auto& hit : cluster.caloHitsPtrVector()){

            int hit_idx = _caloHIs.size();
            _infoStructHelper.fillCaloHitInfo(*hit,_caloHIs,cluster_idx);

            //Update the cluster
            _caloCIs.back().hits_.push_back(hit_idx);

            if (_fillcalorecodigis){
              for (const auto& recodigi : hit->recoCaloDigis()){

                int recodigi_idx = _caloRDIs.size();
                _infoStructHelper.fillCaloRecoDigiInfo(*recodigi,_caloRDIs,hit_idx);

                //Update the hit
                _caloHIs.back().recoDigis_.push_back(recodigi_idx);

                if (_fillcalodigis){
                  const auto& digi = recodigi->caloDigiPtr();

                  int digi_idx = _caloDIs.size();
                  _infoStructHelper.fillCaloDigiInfo(*digi,_caloDIs,recodigi_idx);

                  //Update the recodigi
                  _caloRDIs.back().caloDigiIdx_ = digi_idx;

                }
              }
            }
          }
        }
      }
    } else { //No clusters

      if (_fillcalohits){
        //Get the hits
        event.getByLabel(_conf.caloHitsTag(),_caloHits);
        for (const auto& hit : *_caloHits.product()){

          int hit_idx = _caloHIs.size();
          _infoStructHelper.fillCaloHitInfo(hit,_caloHIs);

          if (_fillcalorecodigis){
            for (const auto& recodigi : hit.recoCaloDigis()){

              int recodigi_idx = _caloRDIs.size();
              _infoStructHelper.fillCaloRecoDigiInfo(*recodigi,_caloRDIs,hit_idx);

              //Update the hit
              _caloHIs.back().recoDigis_.push_back(recodigi_idx);

              if (_fillcalodigis){
                const auto& digi = recodigi->caloDigiPtr();

                int digi_idx = _caloDIs.size();
                _infoStructHelper.fillCaloDigiInfo(*digi,_caloDIs,recodigi_idx);

                //Update the recodigi
                _caloRDIs.back().caloDigiIdx_ = digi_idx;

              }
            }
          }
        }
      } else { //No hits

        if (_fillcalorecodigis){
          //Get the recodigis
          event.getByLabel(_conf.caloRecoDigisTag(),_caloRecoDigis);
          for (const auto& recodigi : *_caloRecoDigis.product()){

            int recodigi_idx = _caloRDIs.size();
            _infoStructHelper.fillCaloRecoDigiInfo(recodigi,_caloRDIs);

            if (_fillcalodigis){
              const auto& digi = recodigi.caloDigiPtr();
              int digi_idx = _caloDIs.size();
              _infoStructHelper.fillCaloDigiInfo(*digi,_caloDIs,recodigi_idx);

              //Update the recodigi
              _caloRDIs.back().caloDigiIdx_ = digi_idx;

            }
          }
        } else { //No recodigis

          if (_fillcalodigis){
            //Get the digis
            event.getByLabel(_conf.caloDigisTag(),_caloDigis);
            for (const auto& digi : *_caloDigis.product()){
              _infoStructHelper.fillCaloDigiInfo(digi,_caloDIs);
            }
          }
        }
      }

    }

    // TODO we want MC information when we don't have a track
    // fill general CRV info
    if(_fillcrvcoincs){
      // clear vectors
      _crvcoincs.clear();
      _crvcoincsmc.clear();
      _crvcoincsmcplane.clear();
      _crvpulses.clear();
      _crvdigis.clear();
      _crvpulsesmc.clear();

      event.getByLabel(_conf.crvCoincidencesTag(),_crvCoincidences);
      event.getByLabel(_conf.crvRecoPulsesTag(),_crvRecoPulses);
      event.getByLabel(_conf.crvStepsTag(),_crvSteps);
      event.getByLabel(_conf.crvDigisTag(),_crvDigis);
      if(_fillmc){
        event.getByLabel(_conf.crvCoincidenceMCsTag(),_crvCoincidenceMCs);
        event.getByLabel(_conf.crvDigiMCsTag(),_crvDigiMCs);
      }
      _crvHelper.FillCrvHitInfoCollections(
                                           _crvCoincidences, _crvCoincidenceMCs,
                                           _crvRecoPulses, _crvSteps, _mcTrajectories,_crvcoincs, _crvcoincsmc,
                                           _crvsummary, _crvsummarymc, _crvcoincsmcplane, _crvPlaneY, _pph);
      if(_fillcrvpulses){
        _crvHelper.FillCrvPulseInfoCollections(_crvRecoPulses, _crvDigiMCs, _ewmh,
                                               _crvpulses, _crvpulsesmc);
      }
      if(_fillcrvdigis){
        _crvHelper.FillCrvDigiInfoCollections(_crvRecoPulses, _crvDigis,
                                              _crvdigis);
      }

    }
    // fill this row in the TTree
    bool fill = true; // default to fliling event
    if(_hastrks && ntrks == 0) { // if we require tracks (_hastrks) but we have none, don't write
      fill = false;
    }
    if (_hascrv && _crvsummary.totalPEs == 0) { // if we require CRV but we have none, don't write
      fill = false;
    }
    if (fill) {
      _ntuple->Fill();
    }
  }


  void EventNtupleMaker::fillEventInfo( const art::Event& event) {
    // fill basic event information
    _einfo.event = event.event();
    _einfo.run = event.run();
    _einfo.subrun = event.subRun();

    if (_recoCountTag != "") {
      auto recoCountHandle = event.getValidHandle<mu2e::RecoCount>(_recoCountTag);
      auto recoCount = *recoCountHandle;
      _infoStructHelper.fillHitCount(recoCount, _hcnt);
    }

    // currently no reco nproton estimate TODO
    if (_PBTTag != "") {
      auto PBThandle = event.getValidHandle<mu2e::ProtonBunchTime>(_PBTTag);
      auto PBT = *PBThandle;
      _einfo.pbtime = PBT.pbtime_;
      _einfo.pbterr = PBT.pbterr_;
    }

    if (_EWMTag != "") {
      event.getByLabel(_EWMTag, _ewmh);
    }

    if (_fillmc) {
      auto PBTMChandle = event.getValidHandle<mu2e::ProtonBunchTimeMC>(_PBTMCTag);
      auto PBTMC = *PBTMChandle;
      _einfomc.pbtime = PBTMC.pbtime_;

      auto PBIhandle = event.getValidHandle<mu2e::ProtonBunchIntensity>(_PBITag);
      auto PBI = *PBIhandle;
      _einfomc.nprotons = PBI.intensity();
    }

    // get event weight products
    std::vector<Float_t> weights;
    for (const auto& i_weightHandle : _wtHandles) {
      double weight = i_weightHandle->weight();
      weights.push_back(weight);
    }
    _wtinfo.setWeights(weights);
  }

  void EventNtupleMaker::fillTriggerBranch(const art::Event& event,std::string const& process, bool firstEvent) {
    art::InputTag const tag{Form("TriggerResults::%s", process.c_str())};
    auto trigResultsH = event.getValidHandle<art::TriggerResults>(tag);
    const art::TriggerResults* trigResults = trigResultsH.product();
    TriggerResultsNavigator tnav(trigResults);

    if (firstEvent) {
      if (tnav.getTrigPaths().size() > TrigInfo::ntrig_) {
        throw cet::exception("EventNtuple") << "More trigger paths in TriggerResultsNavigator than maximum allowed by TrigInfo::ntrig_. Increase TrigInfo::ntrig_ and rebuild\n";
      }
      for (unsigned int i = 0; i < tnav.getTrigPaths().size(); ++i) {
          const std::string name = "trig_"+tnav.getTrigPathName(i);
          _ntuple->Branch(name.c_str(), &_triggerResults._triggerArray[i], _buffsize,_splitlevel);
      }
    }

    for (unsigned int i = 0; i < tnav.getTrigPaths().size(); ++i) {
        const std::string path = tnav.getTrigPathName(i);
        bool accepted = tnav.accepted(path);
        _triggerResults._triggerArray[i] = accepted;
    }


  }

  void EventNtupleMaker::fillTrackBranches(const art::Handle<KalSeedPtrCollection>& kspch, BranchIndex i_branch, size_t i_kseedptr) {

    const auto& kseedptr = (kspch->at(i_kseedptr));
    const auto& kseed = *kseedptr;
    // general info
    _infoStructHelper.fillTrkInfo(kseed,_allTIs.at(i_branch));

    // fit information at specific points:e
    _infoStructHelper.fillTrkSegInfo(kseed,_allTSIs.at(i_branch));
    if(_ftype == LoopHelix && kseed.loopHelixFit())_infoStructHelper.fillLoopHelixInfo(kseed,_allLHIs.at(i_branch));
    if(_ftype == CentralHelix && kseed.centralHelixFit())_infoStructHelper.fillCentralHelixInfo(kseed,_allCHIs.at(i_branch));
    if(_ftype == KinematicLine && kseed.kinematicLineFit())_infoStructHelper.fillKinematicLineInfo(kseed,_allKLIs.at(i_branch));
    BranchConfig branchConfig = _allBranches.at(i_branch);
    if(_conf.diag() > 1 || (_conf.fillhits() && branchConfig.options().fillhits())){ // want hit level info
      _infoStructHelper.fillHitInfo(kseed, _allTSHIs.at(i_branch), _allTSHCIs.at(i_branch), _conf.fillhitcalibs());
      _infoStructHelper.fillMatInfo(kseed, _allTSMIs.at(i_branch));
    }

    // calorimeter info
    _infoStructHelper.fillTrkCaloHitInfo(kseed,  _allTCHIs.at(i_branch)); // fillTrkCaloHitInfo handles whether there is a calo hit or not
    if (kseed.hasCaloCluster()) {
      // test
      if(_conf.debug()>0){
        auto const& tch = kseed.caloHit();
        auto const& cc = tch.caloCluster();
        std::cout << "CaloCluster has energy " << cc->energyDep()
          << " +- " << cc->energyDepErr() << std::endl;
      }
    }


    const auto& trkQualHandles = _allTrkQualCHs.at(i_branch);
    for (size_t i_trkQualHandle = 0; i_trkQualHandle < trkQualHandles.size(); ++i_trkQualHandle) {
      const auto& trkQualHandle = trkQualHandles.at(i_trkQualHandle);
      if (trkQualHandle.isValid()) { // might not have a valid handle
        _infoStructHelper.fillTrkQualInfo(kseed, trkQualHandle->at(i_kseedptr) , _allTrkQualResults.at(i_branch).at(i_trkQualHandle));
      }
    }

    // all RecoQuals
    std::vector<Float_t> recoQuals; // for the output value
    for (const auto& i_recoQualHandle : _allRQCHs.at(i_branch)) {
      Float_t recoQual = i_recoQualHandle->at(i_kseedptr)._value;
      recoQuals.push_back(recoQual);
      Float_t recoQualCalib = i_recoQualHandle->at(i_kseedptr)._calib;
      recoQuals.push_back(recoQualCalib);
    }
    _allRQIs.at(i_branch).setQuals(recoQuals);
    // TrkCaloHitPID
    
    const auto& trkPIDHandles = _allTrkPIDCHs.at(i_branch);
    for (size_t i_trkPIDHandle = 0; i_trkPIDHandle < trkPIDHandles.size(); ++i_trkPIDHandle) {
      const auto& trkPIDHandle = trkPIDHandles.at(i_trkPIDHandle);
      if (trkPIDHandle.isValid()) { // might not have a valid handle
        _infoStructHelper.fillTrkPIDInfo(kseed, trkPIDHandle->at(i_kseedptr) , _allTrkPIDResults.at(i_branch).at(i_trkPIDHandle));
      }
    }

    // fill MC info associated with this track
    if(_fillmc && branchConfig.options().fillmc()) {
      const PrimaryParticle& primary = *_pph;
      // use Assns interface to find the associated KalSeedMC; this uses ptrs
      if(_conf.debug() > 1)std::cout << "KalSeedMCMatch has " << _ksmcah->size() << " entries" << std::endl;
      for(auto iksmca = _ksmcah->begin(); iksmca!= _ksmcah->end(); iksmca++){
        if(_conf.debug() > 2) std::cout << "KalSeed Ptr " << kseedptr << " match Ptr " << iksmca->first << "?" << std::endl;
        if(iksmca->first == kseedptr) {
          auto const& kseedmc = *(iksmca->second);
          _infoMCStructHelper.fillTrkInfoMC(kseed, kseedmc, _surfaceStepsHandle, _allMCTIs.at(i_branch));
          auto& mcvdis = _allMCVDInfos.at(i_branch);
          _infoMCStructHelper.fillVDInfo(kseed, kseedmc, mcvdis);
          _infoMCStructHelper.fillAllSimInfos(kseedmc, primary, _allMCSimTIs.at(i_branch), branchConfig.options().genealogyDepth(), branchConfig.options().matchDepth());

          if(_conf.diag() > 1 || (_conf.fillhits() && branchConfig.options().fillhits())){
            _infoMCStructHelper.fillHitInfoMCs(kseed,kseedmc, _allTSHIMCs.at(i_branch));
          }
          // fill extra MCStep info for this branch
          for(size_t ixt = 0; ixt < _extraMCStepTags.size(); ixt++){
            auto const& mcsc = *_extraMCStepCollections[ixt];
            auto& mcsic = _extraMCStepInfos[i_branch][ixt];
            auto& mcssi = _extraMCStepSummaryInfos.at(i_branch).at(ixt);
            _infoMCStructHelper.fillExtraMCStepInfos(kseedmc,mcsc,mcsic,mcssi);
          }
          // fill SurfaceStep info
          if(_surfaceStepsHandle.isValid()){
            if(_conf.debug() > 2)std::cout << "SurfaceSteps from handle " << _surfaceStepsHandle << std::endl;
            auto& ssi = _surfaceStepInfos.at(i_branch);
            ssi.push_back(std::vector<SurfaceStepInfo>());
            _infoMCStructHelper.fillSurfaceStepInfos(kseedmc,*_surfaceStepsHandle,ssi.back());
          }
          break;
        }
      }
      if (kseed.hasCaloCluster() && _fillcalomc) {
        // fill MC truth of the associated CaloCluster.  Use the fact that these are correlated by index with the clusters in that collection
        auto index = kseed.caloCluster().key();
        auto const& ccmcc = *_ccmcch;
        auto const& ccmc = ccmcc[index];
        _infoMCStructHelper.fillCaloClusterInfoMC(ccmc,_allMCTCHIs.at(i_branch));  // currently broken due to CaloMC changes.  This needs fixing in compression
      }
    }
  }

  // some branches can't be made until the analyze() function because we want to write out all data products of a certain type
  // these all have an underlying array where we want to name the individual elements in the array with different leaf names
  template <typename T, typename TI, typename TIA>
  std::vector<art::Handle<T> >  EventNtupleMaker::createSpecialBranch(const art::Event& event, const std::string& branchname,
  std::vector<art::Handle<T> >& handles, // this parameter is only needed so that the template parameter T can be deduced. There is probably a better way to do this FIXME
  TI& infostruct, TIA& array, bool make_individual_branches, const std::string& selection) {
    std::vector<art::Handle<T> > outputHandles;
    std::vector<art::Handle<T> > inputHandles = event.getMany<T>();
    if (inputHandles.size()>0) {
      std::vector<std::string> labels;
      for (const auto& i_handle : inputHandles) {
        std::string moduleLabel = i_handle.provenance()->moduleLabel();
        // event.getMany() doesn't have a way to wildcard part of the ModuleLabel, do it ourselves here
        size_t pos;
        if (selection != "") { // if we want to add a selection
          pos = moduleLabel.find(selection);

          // make sure that the selection (e.g. "DeM") appears at the end of the module label
          if (pos == std::string::npos) {
            if(_conf.debug() > 3)std::cout << "Selection not found" << std::endl;
            continue;
          }
          else if (pos+selection.length() != moduleLabel.size()) {
            if(_conf.debug() > 3)std::cout << "Selection wasn't at end of moduleLabel" << std::endl;
            continue;
          }
          moduleLabel = moduleLabel.erase(pos, selection.length());
        }
        std::string instanceName = i_handle.provenance()->productInstanceName();

        std::string branchname = moduleLabel;
        if (instanceName != "") {
          branchname += "_" + instanceName;
        }
        outputHandles.push_back(i_handle);
        labels.push_back(branchname);
      }
      if (make_individual_branches) { // if we want to make individual branches per leaf (e.g. to avoid branch ambiguities in python such as detrkqual.NActiveHits vs uetrkqual.NActiveHits)
        const std::vector<std::string>& leafnames = infostruct.leafnames(labels);
        int n_leaves = leafnames.size();
        for (int i_leaf = 0; i_leaf < n_leaves; ++i_leaf) {
          std::string thisbranchname = (branchname+"."+leafnames.at(i_leaf));
          if (!_ntuple->GetBranch(thisbranchname.c_str())) {  // only want to create the branch once
            _ntuple->Branch(thisbranchname.c_str(), &array[i_leaf]);
          }
        }
      }
      else {
        if (!_ntuple->GetBranch((branchname+".").c_str())) {  // only want to create the branch once
          _ntuple->Branch((branchname+".").c_str(), &infostruct, infostruct.leafname(labels).c_str());
        }
      }
    }
    return outputHandles;
  }

  void EventNtupleMaker::resetTrackBranches() {
    for (BranchIndex i_branch = 0; i_branch < _allBranches.size(); ++i_branch) {

      _allRQIs.at(i_branch).reset();
    }
  }
}  // end namespace mu2e

// Part of the magic that makes this class a module.
// create an instance of the module.  It also registers
using mu2e::EventNtupleMaker;
DEFINE_ART_MODULE(EventNtupleMaker)
