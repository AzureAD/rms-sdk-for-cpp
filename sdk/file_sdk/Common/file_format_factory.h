#ifndef FILE_FORMAT_FACTORY_H
#define FILE_FORMAT_FACTORY_H

#include <memory>
#include <IStream.h>
#include "ifile_format.h"

using std::string;
using mip::file::IFileFormat;

namespace mip {
namespace file {

class FileFormatFactory {
private:
  FileFormatFactory();
  static bool in_array(const string &value, const vector<string> &array);

public:
  static std::shared_ptr<IFileFormat> Create(SharedStream stream, const string& extension);
};

} // namespace file
} // namespace mip

#endif // FILE_FORMAT_FACTORY_H
