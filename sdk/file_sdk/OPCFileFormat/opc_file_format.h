#ifndef OPC_FILE_FORMAT_H
#define OPC_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class OPCFileFormat : public FileFormat
{
public:
  OPCFileFormat(shared_ptr<IStream> file, const std::string& extension);
  const vector<pair<std::string, std::string>> GetProperties() override;
  void UpdateProperties(const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove) override;
  void Commit(shared_ptr<IStream> outputStream, string& newExtension) override;
};

} // namespace file
} // namespace mip

#endif // OPC_FILE_FORMAT_H
