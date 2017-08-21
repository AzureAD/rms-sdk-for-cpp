#ifndef XMP_FILE_FORMAT_H
#define XMP_FILE_FORMAT_H

#include <file_format.h>

namespace mip {
namespace file {

class XMPFileFormat : public FileFormat
{
public:
  XMPFileFormat(shared_ptr<IStream> file, const std::string& extension);
  const vector<Tag> ReadTags();
  string GetOriginalExtension();
  void Commit(shared_ptr<IStream> file, string& newExtension);
};

} // namespace file
} // namespace mip
#endif // XMP_FILE_FORMAT_H
