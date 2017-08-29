#include "default_format.h"

namespace mip {
namespace file {

DefaultFormat::DefaultFormat(shared_ptr<IStream> file, const std::string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> DefaultFormat::ReadTags() {
  throw std::exception("not implemented");
}

string DefaultFormat::GetOriginalExtension() {
  return mExtension;
}

void DefaultFormat::Commit(shared_ptr<IStream> file, string& newExtension) {
  throw std::exception("not implemented");
}

} // namespace file
} // namespace mip
