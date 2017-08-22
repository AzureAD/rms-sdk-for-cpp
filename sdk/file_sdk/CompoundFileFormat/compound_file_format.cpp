#include "compound_file_format.h"

namespace mip {
namespace file {

CompoundFileFormat::CompoundFileFormat(shared_ptr<IStream> file, const std::string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> CompoundFileFormat::ReadTags() {
  return vector<Tag>();
}

} // namespace file
} // namespace mip
