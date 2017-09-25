#ifndef DEFAULT_FORMAT_H
#define DEFAULT_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class DefaultFormat : public FileFormat
{

public:
  DefaultFormat(shared_ptr<IStream> file, const string& extension);
  const vector<pair<string, string>> GetProperties() override;
  void UpdateProperties(const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove) override;
  void Commit(shared_ptr<IStream> outputStream, string& newExtension) override;
};

} // namespace file
} // namespace mip

#endif // DEFAULT_FORMAT_H
