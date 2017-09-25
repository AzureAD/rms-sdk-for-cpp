#ifndef FILE_SDK_FILE_FORMAT_H
#define FILE_SDK_FILE_FORMAT_H

#include "ifile_format.h"

using std::shared_ptr;

namespace mip {
namespace file {

class FileFormat : public IFileFormat
{
public:
  string GetOriginalExtension() override { return mExtension; }

protected:
  FileFormat(shared_ptr<IStream> inputStream, const string& extension)
  : mFile(inputStream),
    mExtension(extension) {
}

protected:
  shared_ptr<IStream> mFile;

  string mExtension;
};

} //namespace file
} //namespace mip

#endif // FILE_SDK_FILE_FORMAT_H
