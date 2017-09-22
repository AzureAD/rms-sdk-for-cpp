#ifndef PFIILE_FILE_FORMAT_H
#define PFIILE_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class PFileFileFormat : public FileFormat
{

public:
  PFileFileFormat(std::shared_ptr<IStream> file, const std::string& extension);
  const vector<Tag> ReadTags();
  string GetOriginalExtension();
  void Commit(std::shared_ptr<IStream> outputStream, string& newExtension);
};

} // namespace file
} // namespace mip

#endif // PFILE_FILE_FORMAT_H
