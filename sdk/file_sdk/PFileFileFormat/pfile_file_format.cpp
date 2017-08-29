#include "pfile_file_format.h"

namespace mip {
namespace file {

PFileFileFormat::PFileFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> PFileFileFormat::ReadTags() {
  throw std::runtime_error("not implemented");
}

string PFileFileFormat::GetOriginalExtension() {
  throw std::runtime_error("not implemented");
}

void PFileFileFormat::Commit(shared_ptr<IStream> file, string& newExtension) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
