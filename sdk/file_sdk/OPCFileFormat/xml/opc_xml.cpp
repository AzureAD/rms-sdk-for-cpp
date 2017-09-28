#include "OPCFileFormat/xml/opc_xml.h"

using std::string;

namespace mip {
namespace file {

OpcXml::OpcXml(const string& xml) : mDocument(XmlDocument::ParseXmlDocument(xml)) {
}

string OpcXml::Serialize() const {
  return mDocument.GetXmlContent();
}

} // namespace file
} // namespace mip
