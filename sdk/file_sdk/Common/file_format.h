#ifndef FILE_FORMAT_H
#define FILE_FORMAT_H

#include "ifile_format.h"

using std::shared_ptr;

namespace mip {
namespace file {

class FileFormat : public IFileFormat
{
public:
  virtual string GetOriginalExtension();

  virtual const vector<Tag>& GetTags();

  virtual void SetTags(const vector<Tag>& tags);

  virtual void Commit(shared_ptr<IStream> file, string& newExtension);

protected:
  FileFormat(shared_ptr<IStream> file, const string& extension);

  // Reads the tags from the file
  virtual const vector<Tag>& ReadTags() = 0;

protected:
  shared_ptr<IStream> mFile;

  string mExtension;

  vector<Tag> mTags;

  bool mHasTags;
};

} //namespace file
} //namespace mip

#endif // FILE_FORMAT_H
