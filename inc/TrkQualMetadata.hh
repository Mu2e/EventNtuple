#ifndef TrkQualMetadata_hh
#define TrkQualMetadata_hh

#include <string>

namespace mu2e {
  struct TrkQualMetadata {
    std::string output_branch;
    std::string input_tag;
    std::string model_version;
  };
}

#endif
