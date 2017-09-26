#include "OPCFileFormat/opc_file_format.h"
#include "OPCFileFormat/xml/customproperties.h"
#include "OPCFileFormat/zip_file.h"

namespace {
  string kCustomPropertiesEntry = "docProps/custom.xml";
}

namespace mip {
namespace file {

OPCFileFormat::OPCFileFormat(std::shared_ptr<IStream> inputStream, const string& extension)
  : FileFormat(inputStream, extension) {
}

const vector<pair<std::string, std::string> > OPCFileFormat::GetProperties() {
  ZipFile file(mFile);
  string entry = file.GetEntry(kCustomPropertiesEntry);
  CustomPropertiesXml xml(entry);
  return xml.GetProperties();
}

void OPCFileFormat::Commit(std::shared_ptr<IStream> outputStream, string& newExtension) {
  throw std::runtime_error("not implemented");
}

void OPCFileFormat::UpdateProperties(const std::vector<std::pair<std::string, std::string>>& propertiesToAdd, const std::vector<std::string>&  keysToRemove) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
