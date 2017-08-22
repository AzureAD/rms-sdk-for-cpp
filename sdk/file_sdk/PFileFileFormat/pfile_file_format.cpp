#include "pfile_file_format.h"

namespace mip {
namespace file {

PFileFileFormat::PFileFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> PFileFileFormat::ReadTags() {
  vector<Tag> tmp;
  return tmp;
}

} // namespace file
} // namespace mip
