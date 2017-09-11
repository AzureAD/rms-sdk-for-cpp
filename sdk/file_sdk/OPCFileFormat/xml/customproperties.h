#ifndef CUSTOMPROPERTIES_H
#define CUSTOMPROPERTIES_H

#include "xml/opcxml.h"
#include <string>
#include <vector>

namespace mip {
namespace file {

typedef std::pair<std::string, std::string> CustomProperty;

class CustomPropertiesXml : public OpcXml {
public:
  CustomPropertiesXml(const std::string& xml);
  std::vector<std::pair<std::string, std::string>> GetProperties();
  void UpdateProperties(std::vector<CustomProperty> propertiesToAdd, std::vector<std::string> keysToRemove);
};

} // namespace file
} // namespace mip

#endif // CUSTOMPROPERTIES_H
