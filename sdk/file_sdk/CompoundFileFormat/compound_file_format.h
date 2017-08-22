#ifndef COMPOUND_FILE_FORMAT_H
#define COMPOUND_FILE_FORMAT_H

#include <file_format.h>

namespace mip {
namespace file {

class CompoundFileFormat : public FileFormat
{
public:
  CompoundFileFormat(shared_ptr<IStream> file, const string& extension);
  const vector<Tag> ReadTags();
};

} // namespace file
} // namespace mip

#endif // COMPOUND_FILE_FORMAT_H
