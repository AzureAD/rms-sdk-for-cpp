#include "xmp_file_format.h"

namespace mip {
namespace file {

XMPFileFormat::XMPFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag>& XMPFileFormat::ReadTags() {
  throw std::exception("not implemnted");
}

} // namespace file
} // namespace mip
