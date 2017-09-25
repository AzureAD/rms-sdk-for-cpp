#ifndef PDF_FILE_FORMAT_H
#define PDF_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class PDFFileFormat : public FileFormat
{

public:
  PDFFileFormat(std::shared_ptr<IStream> file, const std::string& extension);
  const vector<pair<std::string, std::string>> GetProperties() override;
  void UpdateProperties(const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove) override;
  void Commit(std::shared_ptr<IStream> outputStream, string& newExtension) override;
};

} // namespace file
} // namespace mip

#endif // PDF_FILE_FORMAT_H
