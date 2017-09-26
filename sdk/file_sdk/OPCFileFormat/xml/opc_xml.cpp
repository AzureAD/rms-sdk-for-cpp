#include "OPCFileFormat/xml/opc_xml.h"

namespace mip {
namespace file {

  using std::string;

OpcXml::OpcXml(const string &xml) : mDocument(XmlDocument::ParseXmlDocument(xml)) {
}

string OpcXml::Serialize() const {
  return mDocument.GetXmlContent();
}

} // namespace file
} // namespace mip
