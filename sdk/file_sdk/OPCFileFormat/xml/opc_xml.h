#ifndef OPCXML_H
#define OPCXML_H

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

#endif // OPCXML_H
