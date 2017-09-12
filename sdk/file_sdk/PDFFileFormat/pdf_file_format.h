#ifndef PDF_FILE_FORMAT_H
#define PDF_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class PDFFileFormat : public FileFormat
{

public:
  PDFFileFormat(SharedStream file, const std::string& extension);
  const vector<Tag> ReadTags();
  void Commit(SharedStream file, string& newExtension);
};

} // namespace file
} // namespace mip

#endif // PDF_FILE_FORMAT_H
