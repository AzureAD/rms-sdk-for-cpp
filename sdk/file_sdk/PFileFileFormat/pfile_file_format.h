#ifndef PFIILE_FILE_FORMAT_H
#define PFIILE_FILE_FORMAT_H

#include <file_format.h>

namespace mip {
namespace file {

class PFileFileFormat : public FileFormat
{

public:
  PFileFileFormat(shared_ptr<IStream> file, const std::string& extension);
  const vector<Tag> ReadTags();
};

} // namespace file
} // namespace mip

#endif // PFILE_FILE_FORMAT_H
