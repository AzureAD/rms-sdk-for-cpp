#ifndef DEFAULT_FORMAT_H
#define DEFAULT_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class DefaultFormat : public FileFormat
{

public:
  DefaultFormat(shared_ptr<IStream> file, const std::string& extension);
  const vector<Tag> ReadTags();
  void Commit(shared_ptr<IStream> file, string& newExtension);
};

} // namespace file
} // namespace mip

#endif // DEFAULT_FORMAT_H
