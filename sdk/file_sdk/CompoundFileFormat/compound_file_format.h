#ifndef COMPOUND_FILE_FORMAT_H
#define COMPOUND_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class CompoundFileFormat : public FileFormat
{

public:
  CompoundFileFormat(shared_ptr<IStream> inputStream, const string& extension);
  const vector<pair<string, string>> GetProperties() override;
  void UpdateProperties(const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove) override;
  void Commit(shared_ptr<IStream> outputStream, string& newExtension) override;
};

} // namespace file
} // namespace mip

#endif // COMPOUND_FILE_FORMAT_H
