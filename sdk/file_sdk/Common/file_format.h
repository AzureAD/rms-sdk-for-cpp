#ifndef FILE_SDK_FILE_FORMAT_H
#define FILE_SDK_FILE_FORMAT_H

#include "ifile_format.h"

using std::shared_ptr;

namespace mip {
namespace file {

class FileFormat : public IFileFormat
{
public:
  string GetOriginalExtension() override;

  const vector<Tag> GetTags() override;

  void SetTags(const vector<Tag>& tags) override;

  void Commit(shared_ptr<IStream> file, string& newExtension) override = 0;

protected:
  FileFormat(shared_ptr<IStream> file, const string& extension);

  // Reads the tags from the file
  virtual const vector<Tag> ReadTags() = 0;

protected:
  shared_ptr<IStream> mFile;

  string mExtension;

  vector<Tag> mTags;

  bool mHasTags;
};

} //namespace file
} //namespace mip

#endif // FILE_SDK_FILE_FORMAT_H
