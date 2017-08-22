#ifndef FILE_FORMAT_FACTORY_H
#define FILE_FORMAT_FACTORY_H

#include <memory>
#include <IStream.h>
#include "ifile_format.h"

using std::string;
using rmscrypto::api::IStream;
using mip::file::IFileFormat;

namespace mip {
namespace file {

class FileFormatFactory {
private:
    FileFormatFactory();

public:
  static std::shared_ptr<IFileFormat> Create(std::shared_ptr<IStream> fileStream, const string& extension);
};

} // namespace file
} // namespace mip

#endif // FILE_FORMAT_FACTORY_H
