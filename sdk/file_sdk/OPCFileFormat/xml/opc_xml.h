#ifndef OPCFILEFORMAT_XML_OPC_XML_H
#define OPCFILEFORMAT_XML_OPC_XML_H

#include <string>
#include "OPCFileFormat/xml/xml_document.h"

namespace mip {
namespace file {

class OpcXml {
public:
  std::string Serialize() const;

protected:
  OpcXml(const std::string& xml);

  XmlDocument mDocument;
};

} // namespace file
} // namespace mip

#endif // OPCFILEFORMAT_XML_OPC_XML_H
