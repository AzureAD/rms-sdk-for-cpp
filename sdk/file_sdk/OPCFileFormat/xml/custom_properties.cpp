#include "OPCFileFormat/xml/custom_properties.h"

namespace mip {
namespace file {

CustomPropertiesXml::CustomPropertiesXml(const std::string &xml)
    : OpcXml(xml) {
}

std::vector<CustomProperty> CustomPropertiesXml::GetProperties()
{
  std::vector<CustomProperty> result;
  for (auto node = mDocument.GetRootNode().GetFirstChild(); !(node == XmlNode()); node = node.GetNextNode()) {
    auto subNode = node.GetFirstChild();
    CustomProperty cp = std::make_pair(node.GetAttributeValue("name"), subNode.GetNodeInnerText());
    result.push_back(cp);
  }
  return result;
}

void CustomPropertiesXml::UpdateProperties(std::vector<CustomProperty> propertiesToAdd, std::vector<std::string> keysToRemove) {

}

} // namespace file
} // namespace mip
