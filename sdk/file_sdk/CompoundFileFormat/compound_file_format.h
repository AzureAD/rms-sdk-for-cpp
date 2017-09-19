#ifndef COMPOUND_FILE_FORMAT_H
#define COMPOUND_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class CompoundFileFormat : public FileFormat
{

public:
  CompoundFileFormat(std::shared_ptr<IStream> inputStream, const string& extension);
  const vector<Tag> ReadTags();
  void Commit(std::shared_ptr<IStream> outputStream, string& newExtension);
};

} // namespace file
} // namespace mip

#endif // COMPOUND_FILE_FORMAT_H
