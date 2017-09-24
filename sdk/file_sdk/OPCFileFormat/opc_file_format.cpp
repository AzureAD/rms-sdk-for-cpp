#include "OPCFileFormat/opc_file_format.h"
#include "OPCFileFormat/xml/customproperties.h"
#include "OPCFileFormat/zip_file.h"

namespace {
  string kCustomPropertiesEntry = "docProps/custom.xml";
}

namespace mip {
namespace file {

OPCFileFormat::OPCFileFormat(std::shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension) {
}

const vector<Tag> OPCFileFormat::ReadTags() {
  ZipFile file(mFile);
  string entry = file.GetEntry(kCustomPropertiesEntry);
  CustomPropertiesXml xml(entry);
  auto props = xml.GetProperties();
  return Tag::FromProperties(props);
}

void OPCFileFormat::Commit(std::shared_ptr<IStream> file, string& newExtension) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
