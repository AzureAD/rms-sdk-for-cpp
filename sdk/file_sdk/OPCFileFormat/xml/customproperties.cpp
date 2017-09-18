#include "OPCFileFormat/xml/customproperties.h"

namespace mip {
namespace file {

CustomPropertiesXml::CustomPropertiesXml(const std::string &xml)
    : OpcXml(xml) {
}

std::vector<std::pair<std::string, std::string>> CustomPropertiesXml::GetProperties()
{
	return {};
}

void CustomPropertiesXml::UpdateProperties(std::vector<CustomProperty> propertiesToAdd, std::vector<std::string> keysToRemove) {

}

} // namespace file
} // namespace mip
