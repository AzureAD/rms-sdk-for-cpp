#ifndef OPC_FILE_FORMAT_H
#define OPC_FILE_FORMAT_H

#include <file_format.h>

namespace mip {
namespace file {

class OPCFileFormat : public FileFormat
{
public:
  OPCFileFormat(shared_ptr<IStream> file, const std::string& extension);
  const vector<Tag> ReadTags();
  void Commit(shared_ptr<IStream> file, string& newExtension);
};

} // namespace file
} // namespace mip

#endif // OPC_FILE_FORMAT_H
