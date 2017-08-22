#include "file_format.h"

namespace mip {
namespace file {

FileFormat::FileFormat(
    std::shared_ptr<IStream> file,
    const string& extension)
  : mFile(file),
    mExtension(extension)
{
}

const vector<mip::Tag>& mip::file::FileFormat::GetTags()
{
  return mTags;
}

void mip::file::FileFormat::SetTags(const std::vector<mip::Tag>&tags)
{
  mTags = tags;
}

} //namespace file
} //namespace mip
