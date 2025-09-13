cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

find_package(cetmodules)

project(event-ntuple VERSION 6.1.1)

include(CetCMakeEnv)
cet_cmake_env()
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

cet_set_compiler_flags(DIAGS VIGILANT
 WERROR
 NO_UNDEFINED
 EXTRA_FLAGS -pedantic
 -Wno-unused-parameter -Wno-type-limits -Wno-ignored-qualifiers -Wno-unused-but-set-parameter -Wno-extra -Wno-non-virtual-dtor -Wno-implicit-fallthrough -Wno-deprecated-copy
)

file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/EventNtuple SYMBOLIC)


find_package(Offline REQUIRED EXPORT)
find_package(art_root_io REQUIRED EXPORT)

include(ArtDictionary)
include(BuildPlugins)


add_subdirectory(src)

install_headers(SUBDIRNAME EventNtuple SUBDIRS inc)
install_headers(SUBDIRNAME EventNtuple SUBDIRS rooutil/inc)


install(DIRECTORY utils DESTINATION python)
install(DIRECTORY tutorial doc validation DESTINATION ${CMAKE_INSTALL_DATAROOTDIR})

install_source(SUBDIRS src)
install_headers(NO_RELATIVE_SUBDIR SUBDIRS inc SUBDIRNAME EventNtuple)
install_fhicl(SUBDIRS fcl SUBDIRNAME EventNtuple/fcl)

cet_cmake_config()
