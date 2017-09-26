#include "OPCFileFormat/opc_file_format.h"
#include "OPCFileFormat/xml/custom_properties.h"
#include "OPCFileFormat/zip_file.h"

namespace {
  string kCustomPropertiesEntry = "docProps/custom.xml";
}

namespace mip {
namespace file {

OPCFileFormat::OPCFileFormat(shared_ptr<IStream> inputStream, const string& extension)
  : FileFormat(inputStream, extension) {
}

void OPCFileFormat::ReadCustomProperties() {
  ZipFile file(mFile);
  string entry = file.GetEntry(kCustomPropertiesEntry);
  mCustomProperties = std::make_shared<CustomPropertiesXml>(entry);
}

const vector<pair<std::string, std::string> > OPCFileFormat::GetProperties() {
  ReadCustomProperties();
  auto props = mCustomProperties->GetProperties();
  return props;
}

void OPCFileFormat::Commit(std::shared_ptr<IStream> outputStream, string& newExtension) {
  throw std::runtime_error("not implemented");
}

void OPCFileFormat::UpdateProperties(const std::vector<std::pair<std::string, std::string>>& propertiesToAdd, const std::vector<std::string>&  keysToRemove) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
