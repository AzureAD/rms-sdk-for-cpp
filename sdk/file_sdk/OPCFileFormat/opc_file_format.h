#ifndef OPC_FILE_FORMAT_H
#define OPC_FILE_FORMAT_H

#include <Common/file_format.h>

#include <memory>

namespace mip {
namespace file {

using std::shared_ptr;

class OPCFileFormat : public FileFormat 
{
public:
  OPCFileFormat(shared_ptr<IStream> file, const std::string& extension);
  void Commit(shared_ptr<IStream> file, string& newExtension) override;

protected:
  const vector<Tag> ReadTags() override;
};

} // namespace file
} // namespace mip

#endif // OPC_FILE_FORMAT_H
