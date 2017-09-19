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

const vector<Tag> XMPFileFormat::ReadTags() {
  return XMPHelper::GetInstance().GetTags(mFile);
}

void XMPFileFormat::Commit(shared_ptr<IStream> outputStream, string& newExtension) {
  std::string tempFileName = std::tmpnam(nullptr);
//  std::ofstream temp(tempFileName); //create empty file
//  temp.close();
  try {
  int readCount = 0;
  do{
    uint8_t buffer[BufferSize];
    readCount = mFile->Read(&buffer[0], BufferSize);
    outputStream->Write(buffer, readCount);
  }
  while (readCount > 0);

  outputStream->Seek(0);
  XMPHelper::GetInstance().SetTags(outputStream, mTags);

  newExtension = mExtension;
  }
  catch(std::exception ex){
     std::remove(tempFileName.c_str());
     throw ex;
  }

  std::remove(tempFileName.c_str());
}

} // namespace file
} // namespace mip
