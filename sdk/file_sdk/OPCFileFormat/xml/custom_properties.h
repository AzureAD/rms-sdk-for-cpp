#ifndef OPCFILEFORMAT_XML_CUSTOM_PROPERTIES_H
#define OPCFILEFORMAT_XML_CUSTOM_PROPERTIES_H

#include "OPCFileFormat/xml/opc_xml.h"

#include <string>
#include <vector>

namespace mip {
namespace file {

using CustomProperty = std::pair<std::string, std::string>;

class CustomPropertiesXml : public OpcXml {
public:
  CustomPropertiesXml(const std::string& xml);
  std::vector<CustomProperty> GetProperties() const;
  void UpdateProperties(std::vector<CustomProperty>& propertiesToAdd, std::vector<std::string>& keysToRemove);
};

} // namespace file
} // namespace mip

#endif // OPCFILEFORMAT_XML_CUSTOM_PROPERTIES_H
