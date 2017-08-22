#include "default_format.h"

namespace mip {
namespace file {

DefaultFormat::DefaultFormat(shared_ptr<IStream> file, const std::string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> DefaultFormat::ReadTags() {
  return vector<Tag>();
}

} // namespace file
} // namespace mip
