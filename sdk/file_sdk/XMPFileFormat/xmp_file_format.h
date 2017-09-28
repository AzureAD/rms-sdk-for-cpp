#ifndef XMP_FILE_FORMAT_H
#define XMP_FILE_FORMAT_H

#define TXMP_STRING_TYPE std::string
#define XMP_INCLUDE_XMPFILES 1

#include <Common/file_format.h>

namespace mip {
namespace file {

class XMPFileFormat : public FileFormat
{
public:
  XMPFileFormat(shared_ptr<IStream> file, const string& extension);
  const vector<pair<string, string>> GetProperties() override;
  void UpdateProperties(const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove) override;
  void Commit(shared_ptr<IStream> outputStream, string& newExtension) override;

private:
  vector<string> mKeysToRemove;
  vector<pair<string, string>> mPropertiesToAdd;
};

} // namespace file
} // namespace mip

#endif // XMP_FILE_FORMAT_H
