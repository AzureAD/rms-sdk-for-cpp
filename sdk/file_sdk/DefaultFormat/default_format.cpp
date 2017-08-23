#include "default_format.h"

namespace mip {
namespace file {

DefaultFormat::DefaultFormat(shared_ptr<IStream> file, const std::string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag>& DefaultFormat::ReadTags() {
  throw std::exception("not implemnted");
}

} // namespace file
} // namespace mip
