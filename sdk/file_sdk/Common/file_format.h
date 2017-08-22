#ifndef FILE_FORMAT_H
#define FILE_FORMAT_H

#include "ifile_format.h"

namespace mip{
namespace file {

class FileFormat : public IFileFormat
{
public:
  FileFormat(std::shared_ptr<IStream> file,
             const string& extension);
  string GetOriginalExtension();

  void SetTags(const vector<Tag>& tags);

  const vector<Tag>& GetTags();

  void Commit(std::shared_ptr<IStream> file, string& extension);
};

} //namespace file
} //namespace mip

#endif // FILE_FORMAT_H
