#include "compound_file_format.h"

namespace mip {
namespace file {

CompoundFileFormat::CompoundFileFormat(std::shared_ptr<IStream> file, const std::string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> CompoundFileFormat::ReadTags() {
  throw std::runtime_error("not implemented");
}

void CompoundFileFormat::Commit(std::shared_ptr<IStream> file, string& newExtension) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
