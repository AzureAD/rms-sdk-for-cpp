#ifndef OPC_FILE_FORMAT_H
#define OPC_FILE_FORMAT_H

#include <Common/file_format.h>

#include <memory>

namespace mip {
namespace file {

using std::shared_ptr;

class OPCFileFormat : public FileFormat {
public:
  OPCFileFormat(shared_ptr<IStream> file, const string& extension);
  const vector<pair<string, string>> GetProperties() override;
  void UpdateProperties(const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove) override;
  void Commit(shared_ptr<IStream> outputStream, string& newExtension) override;
};

} // namespace file
} // namespace mip

#endif // OPC_FILE_FORMAT_H
