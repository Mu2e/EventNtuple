#ifndef EventNtuple_TrkQualMetadata_hh
#define EventNtuple_TrkQualMetadata_hh

#include <string>

namespace mu2e {
  struct TrkQualMetadata {
    std::string output_branch;
    std::string input_tag;
    std::string model_version;
  };
}

#endif
