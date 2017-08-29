#include "default_format.h"

namespace mip {
namespace file {

DefaultFormat::DefaultFormat(shared_ptr<IStream> file, const std::string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> DefaultFormat::ReadTags() {
  throw std::runtime_error("not implemented");
}

void DefaultFormat::Commit(shared_ptr<IStream> file, string& newExtension) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
