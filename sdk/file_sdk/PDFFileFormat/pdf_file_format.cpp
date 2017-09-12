#include "pdf_file_format.h"

namespace mip {
namespace file {

PDFFileFormat::PDFFileFormat(SharedStream file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> PDFFileFormat::ReadTags() {
  throw std::runtime_error("not implemented");
}

void PDFFileFormat::Commit(SharedStream file, string& newExtension) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
