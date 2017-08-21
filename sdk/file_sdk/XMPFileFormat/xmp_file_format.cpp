#include "xmp_file_format.h"

namespace mip {
namespace file {

XMPFileFormat::XMPFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> XMPFileFormat::ReadTags() {
  return mTags;
}

string XMPFileFormat::GetOriginalExtension() {
  throw std::exception("not implemnted");
}

void XMPFileFormat::Commit(shared_ptr<IStream> file, string& newExtension) {
  throw std::exception("not implemnted");
}

} // namespace file
} // namespace mip
