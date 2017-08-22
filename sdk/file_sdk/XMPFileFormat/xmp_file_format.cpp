#include "xmp_file_format.h"

namespace mip {
namespace file {

XMPFileFormat::XMPFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> XMPFileFormat::ReadTags() {
  return vector<Tag>();
}

} // namespace file
} // namespace mip
