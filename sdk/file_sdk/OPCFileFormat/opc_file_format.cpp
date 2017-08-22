#include "opc_file_format.h"

namespace mip {
namespace file {

OPCFileFormat::OPCFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> OPCFileFormat::ReadTags() {
  return vector<Tag>();
}

} // namespace file
} // namespace mip
