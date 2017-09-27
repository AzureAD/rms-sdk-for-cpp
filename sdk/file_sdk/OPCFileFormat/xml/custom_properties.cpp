#include "OPCFileFormat/xml/custom_properties.h"

using std::string;
using std::vector;

namespace mip {
namespace file {

CustomPropertiesXml::CustomPropertiesXml(const string &xml)
    : OpcXml(xml) {
}

vector<CustomProperty> CustomPropertiesXml::GetProperties() const {
  vector<CustomProperty> result;
  for (auto node = mDocument.GetRootNode().GetFirstChild(); !(node == XmlNode()); node = node.GetNextNode()) {
    auto subNode = node.GetFirstChild();
    result.push_back(make_pair(node.GetAttributeValue("name"), subNode.GetNodeInnerText()));
  }
  return result;
}

void CustomPropertiesXml::UpdateProperties(vector<CustomProperty> propertiesToAdd, vector<string> keysToRemove) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
