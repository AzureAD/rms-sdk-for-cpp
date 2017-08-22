#ifndef FILE_FORMAT_H
#define FILE_FORMAT_H

#include "ifile_format.h"

namespace mip {
namespace file {

class FileFormat : public IFileFormat
{
public:
  FileFormat(std::shared_ptr<IStream> file,
             const string& extension);

  virtual string GetOriginalExtension();

  virtual const vector<Tag>& GetTags();

  virtual void SetTags(const vector<Tag>& tags);

  virtual void Commit(std::shared_ptr<IStream> file, string& newExtension);

private:
  std::shared_ptr<IStream> mFile;

  string mExtension;

  vector<Tag> mTags;
};

} //namespace file
} //namespace mip

#endif // FILE_FORMAT_H
