cet_make_library(
    SOURCE
      CrvHitInfoReco.cc
      SConscript
      InfoStructHelper.cc
      CrvInfoHelper.cc
      classes.h
      CrvHitInfoMC.cc
      CrvPlaneInfoMC.cc
      classes_def.xml
      CrvPulseInfoReco.cc
      CrvSummaryMC.cc
      SurfaceStepInfo.cc
      InfoMCStructHelper.cc
    LIBRARIES PUBLIC
      KinKal_Trajectory
      
      Offline::CRVReco
      Offline::CRVResponse
      Offline::MCDataProducts
      Offline::RecoDataProducts
      Offline::DataProducts
      Offline::KinKalGeom
      Offline::Mu2eKinKal
      Offline::TrackerConditions
      Offline::Mu2eUtilities
      Offline::GeneralUtilities

    LIBRARY_NAME eventntuple
)

art_dictionary( NO_CHECK_CLASS_VERSION # For some reason this segfaults
    CLASSES_DEF_XML ${CMAKE_CURRENT_SOURCE_DIR}/classes_def.xml
    CLASSES_H ${CMAKE_CURRENT_SOURCE_DIR}/classes.h
     DICTIONARY_LIBRARIES
      event-ntuple::eventntuple
      Offline::MCDataProducts
      Offline::RecoDataProducts
      Offline::DataProducts
      Offline::Mu2eUtilities
      Offline::GeneralUtilities
)

cet_build_plugin(EventNtupleMaker art::module
    REG_SOURCE EventNtupleMaker_module.cc
    LIBRARIES REG
      event-ntuple::eventntuple
      Offline::MCDataProducts
      Offline::RecoDataProducts
      Offline::DataProducts
      Offline::Mu2eUtilities
      Offline::GeneralUtilities
      Offline::RecoDataProducts
      art_root_io::TFileService_service
      ROOT::Postscript
)

cet_build_plugin(BestCrvHitDeltaT art::module
    REG_SOURCE BestCrvHitDeltaT_module.cc
    LIBRARIES REG
      event-ntuple::eventntuple
      Offline::MCDataProducts
      Offline::RecoDataProducts
      Offline::DataProducts
      Offline::Mu2eUtilities
      Offline::GeneralUtilities
      Offline::RecoDataProducts
      art_root_io::TFileService_service
      ROOT::Postscript
)

install_source(SUBDIRS src)
