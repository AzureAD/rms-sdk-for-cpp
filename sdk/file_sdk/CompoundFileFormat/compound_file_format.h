#ifndef COMPOUND_FILE_FORMAT_H
#define COMPOUND_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class CompoundFileFormat : public FileFormat
{

public:
  CompoundFileFormat(SharedStream file, const string& extension);
  const vector<Tag> ReadTags();
  void Commit(SharedStream file, string& newExtension);
};

} // namespace file
} // namespace mip

#endif // COMPOUND_FILE_FORMAT_H
