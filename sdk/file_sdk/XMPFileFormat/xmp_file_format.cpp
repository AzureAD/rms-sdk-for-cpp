#include "xmp_file_format.h"
#include "xmp_helper.h"

#include "xmpio_over_istream.h"
#include <XMP.incl_cpp>
#include <XMP.hpp>
#include <time.h>
#include <Common/string_utils.h>
#include <Common/std_stream_adapter.h>
#include <algorithm>
#include <fstream>

using std::string;
using std::vector;

namespace mip {
namespace file {

#define BufferSize 1024*64

XMPFileFormat::XMPFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<pair<std::string, std::string>> XMPFileFormat::GetProperties() {
  return XMPHelper::GetInstance().GetProperties(mFile);
}

void XMPFileFormat::Commit(shared_ptr<IStream> outputStream, string& newExtension) {
  int64_t readCount = 0;
  mFile->Seek(0);
  uint8_t buffer[BufferSize];
  do{
    readCount = mFile->Read(&buffer[0], BufferSize);
    outputStream->Write(buffer, readCount);
  }
  while (readCount > 0);

  outputStream->Seek(0);
  XMPHelper::GetInstance().UpdateProperties(outputStream, mPropertiesToAdd, mKeysToRemove);

  newExtension = mExtension;
}

void XMPFileFormat::UpdateProperties(const std::vector<std::pair<std::string, std::string>>& propertiesToAdd, const std::vector<std::string>& keysToRemove)
{
  mPropertiesToAdd = propertiesToAdd;
  mKeysToRemove = keysToRemove;
}

} // namespace file
} // namespace mip
