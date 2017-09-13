#include "opc_file_format.h"

namespace mip {
namespace file {

OPCFileFormat::OPCFileFormat(std::shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> OPCFileFormat::ReadTags() {
  throw std::runtime_error("not implemented");
}

void OPCFileFormat::Commit(std::shared_ptr<IStream> file, string& newExtension) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
