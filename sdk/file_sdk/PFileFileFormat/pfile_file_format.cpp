#include "pfile_file_format.h"

namespace mip {
namespace file {

PFileFileFormat::PFileFileFormat(SharedStream file, const string& extension)
  : FileFormat(file, extension) {
}

const vector<Tag> PFileFileFormat::ReadTags() {
  return mTags;
}

string PFileFileFormat::GetOriginalExtension() {
  throw std::runtime_error("not implemented");
}

void PFileFileFormat::Commit(SharedStream file, string& newExtension) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
