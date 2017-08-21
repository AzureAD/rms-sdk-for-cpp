#include "file_format_base.h"

namespace mip{
namespace file {

IFileFormat::IFileFormat(
    rmscrypto::api::SharedStream& file,
    string extension)
  :
    mExtension(extension),
    mFile(file)
{
}

void mip::file::IFileFormat::SetTags(std::vector<mip::Tag> &tags)
{
  mTags = tags;
}

vector<Tag> mip::file::IFileFormat::GetTags()
{
  return mTags;
}

} //namespace file
} //namespace mip
