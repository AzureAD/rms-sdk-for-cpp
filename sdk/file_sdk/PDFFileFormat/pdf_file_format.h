#ifndef PDF_FILE_FORMAT_H
#define PDF_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class PDFFileFormat : public FileFormat
{

public:
  PDFFileFormat(std::shared_ptr<IStream> file, const std::string& extension);
  const vector<Tag> ReadTags();
  void Commit(std::shared_ptr<IStream> file, string& newExtension);
};

} // namespace file
} // namespace mip

#endif // PDF_FILE_FORMAT_H
