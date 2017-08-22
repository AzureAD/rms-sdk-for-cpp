#include "file_format.h"

namespace mip{
namespace file {

FileFormat::FileFormat(
    std::shared_ptr<IStream> file,
    const string& extension)
{
  mExtension = extension;
  mFile = file;
}

void mip::file::FileFormat::SetTags(const std::vector<mip::Tag>&tags)
{
  mTags = tags;
}

const vector<mip::Tag>& mip::file::FileFormat::GetTags()
{
  return mTags;
}

} //namespace file
} //namespace mip
