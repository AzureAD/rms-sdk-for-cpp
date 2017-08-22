#ifndef IFILE_FORMAT_H
#define IFILE_FORMAT_H

#include <string>
#include <vector>
#include <IStream.h>
#include "tag.h"

using std::string;
using std::vector;
using rmscrypto::api::IStream;

namespace mip{
namespace file {

class IFileFormat
{
public:

  virtual string GetOriginalExtension() = 0;

  virtual void SetTags(const vector<Tag>& tags) = 0;

  virtual const vector<Tag>& GetTags() = 0;

  virtual void Commit(std::shared_ptr<IStream> file, string& extension) = 0;

protected:

  string mExtension;

  std::shared_ptr<IStream> mFile;

  vector<Tag> mTags;
};

} //namespace file
} //namespace mip

#endif // FILE_FORMAT_BASE_H
