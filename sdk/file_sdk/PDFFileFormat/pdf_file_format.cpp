#include "pdf_file_format.h"

namespace mip {
namespace file {

PDFFileFormat::PDFFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag>& PDFFileFormat::ReadTags() {
  throw std::exception("not implemnted");
}

} // namespace file
} // namespace mip
