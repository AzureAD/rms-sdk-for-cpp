#include "compound_file_format.h"

namespace mip {
namespace file {

CompoundFileFormat::CompoundFileFormat(shared_ptr<IStream> file, const std::string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> CompoundFileFormat::ReadTags() {
  throw std::exception("not implemented");
}

string CompoundFileFormat::GetOriginalExtension() {
  return mExtension;
}

void CompoundFileFormat::Commit(shared_ptr<IStream> file, string& newExtension) {
  throw std::exception("not implemented");
}

} // namespace file
} // namespace mip
