#ifndef PFILE_FILE_FORMAT_H
#define PFILE_FILE_FORMAT_H

#include <Common/file_format.h>

namespace mip {
namespace file {

class PFileFileFormat : public FileFormat
{
public:
  PFileFileFormat(shared_ptr<IStream> file, const string& extension);
  string GetOriginalExtension() override;
  void Commit(shared_ptr<IStream> file, string& newExtension) override;

protected:
  const vector<Tag> ReadTags() override;
};

} // namespace file
} // namespace mip

#endif // PFILE_FILE_FORMAT_H
