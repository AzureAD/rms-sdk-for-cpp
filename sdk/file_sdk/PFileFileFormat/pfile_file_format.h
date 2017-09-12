#ifndef PFIILE_FILE_FORMAT_H
#define PFIILE_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class PFileFileFormat : public FileFormat
{

public:
  PFileFileFormat(SharedStream file, const std::string& extension);
  const vector<Tag> ReadTags();
  string GetOriginalExtension();
  void Commit(SharedStream file, string& newExtension);
};

} // namespace file
} // namespace mip

#endif // PFILE_FILE_FORMAT_H
