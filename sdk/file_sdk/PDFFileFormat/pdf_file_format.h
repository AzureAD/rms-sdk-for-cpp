#ifndef PDF_FILE_FORMAT_H
#define PDF_FILE_FORMAT_H

#include <file_format.h>

namespace mip {
namespace file {

class PDFFileFormat : public FileFormat
{

public:
  PDFFileFormat(shared_ptr<IStream> file, const std::string& extension);
  const vector<Tag>& ReadTags();
};

} // namespace file
} // namespace mip

#endif // PDF_FILE_FORMAT_H
