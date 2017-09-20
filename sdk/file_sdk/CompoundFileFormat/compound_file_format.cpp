#include "compound_file_format.h"

namespace mip {
namespace file {

CompoundFileFormat::CompoundFileFormat(shared_ptr<IStream> file, const std::string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> CompoundFileFormat::ReadTags() {
  throw std::runtime_error("not implemented");
}

void CompoundFileFormat::Commit(shared_ptr<IStream> outputStream, string& newExtension) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
