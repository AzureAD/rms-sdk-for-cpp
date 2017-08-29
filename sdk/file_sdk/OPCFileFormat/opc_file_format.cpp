#include "opc_file_format.h"

namespace mip {
namespace file {

OPCFileFormat::OPCFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> OPCFileFormat::ReadTags() {
  throw std::exception("not implemented");
}

string OPCFileFormat::GetOriginalExtension() {
  return mExtension;
}

void OPCFileFormat::Commit(shared_ptr<IStream> file, string& newExtension) {
  throw std::exception("not implemented");
}

} // namespace file
} // namespace mip
