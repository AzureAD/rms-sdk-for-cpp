#ifndef XMP_FILE_FORMAT_H
#define XMP_FILE_FORMAT_H

#define TXMP_STRING_TYPE std::string
#define XMP_INCLUDE_XMPFILES 1

#include <file_format.h>

namespace mip {
namespace file {

class XMPFileFormat : public FileFormat
{
public:
  XMPFileFormat(shared_ptr<IStream> file, const std::string& extension);
  const vector<Tag> ReadTags();
  void Commit(shared_ptr<IStream> file, string& newExtension);
};

} // namespace file
} // namespace mip
#endif // XMP_FILE_FORMAT_H
