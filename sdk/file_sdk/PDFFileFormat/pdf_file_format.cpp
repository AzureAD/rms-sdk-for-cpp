#include "pdf_file_format.h"

namespace mip {
namespace file {

PDFFileFormat::PDFFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> PDFFileFormat::ReadTags() {
  return vector<Tag>();
}

} // namespace file
} // namespace mip
