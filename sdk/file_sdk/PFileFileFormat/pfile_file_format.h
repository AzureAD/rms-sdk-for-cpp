#ifndef PFIILE_FILE_FORMAT_H
#define PFIILE_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class PFileFileFormat : public FileFormat
{

public:
  PFileFileFormat(std::shared_ptr<IStream> file, const std::string& extension);
  string GetOriginalExtension() override;
  const vector<pair<std::string, std::string>> GetProperties() override;
  void UpdateProperties(const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove) override;
  void Commit(std::shared_ptr<IStream> outputStream, string& newExtension) override;
};

} // namespace file
} // namespace mip

#endif // PFILE_FILE_FORMAT_H
