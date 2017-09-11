#include "xmp_file_format.h"
#include "xmp_helper.h"

#include "xmpio_over_istream.h"
#include <XMP.incl_cpp>
#include <XMP.hpp>
#include <time.h>
#include <string_utils.h>

using std::string;
using std::vector;

namespace mip {
namespace file {

XMPFileFormat::XMPFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> XMPFileFormat::ReadTags() {
  return XMPHelper::GetInstance().GetTags(mFile);
}

void XMPFileFormat::Commit(shared_ptr<IStream> file, string& /*newExtension*/) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
