#ifndef OPCFILEFORMAT_XML_CUSTOM_PROPERTIES_H
#define OPCFILEFORMAT_XML_CUSTOM_PROPERTIES_H

#include "OPCFileFormat/xml/opc_xml.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace mip {
namespace file {

using CustomProperty = std::pair<string, string>;

class CustomPropertiesXml : public OpcXml {
public:
  CustomPropertiesXml(const string& xml);
  vector<CustomProperty> GetProperties() const;
  void UpdateProperties(vector<CustomProperty> propertiesToAdd, vector<string> keysToRemove);
};

} // namespace file
} // namespace mip

#endif // OPCFILEFORMAT_XML_CUSTOM_PROPERTIES_H
