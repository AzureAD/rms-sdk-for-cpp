#include "OPCFileFormat/xml/custom_properties.h"
#include <unordered_set>

using std::string;
using std::vector;

namespace mip {
namespace file {

CustomPropertiesXml::CustomPropertiesXml(const string& xml)
    : OpcXml(xml) {
}

vector<CustomProperty> CustomPropertiesXml::GetProperties() const {
  vector<CustomProperty> result;
  for (auto node = mDocument.GetRootNode().GetFirstChild(); !(node == XmlNode()); node = node.GetNextNode()) {
    if (node.GetNodeName() == "property") {
      auto subNode = node.GetFirstChild();
      if (subNode.GetNodeNamespace() == "vt") {
        result.push_back(make_pair(node.GetAttributeValue("name"), subNode.GetNodeInnerText()));
      }
    }
  }
  return result;
}

void CustomPropertiesXml::UpdateProperties(vector<CustomProperty>& propertiesToAdd, vector<string>& keysToRemove) {
  // Remove keys
  for (const auto& keyToRemove : keysToRemove){
    mDocument.SelectNode("/*[local-name()='Properties']/*[local-name()='property' and @name='" + keyToRemove + "']").Delete();
  }

  // Add properties
  auto rootNode = mDocument.GetRootNode();
  for (const auto& propertyToAdd : propertiesToAdd) {
    auto node = rootNode.AddNewChild("property");
    node.AddAttribute("fmtid", "{D5CDD505-2E9C-101B-9397-08002B2CF9AE}");
    node.AddAttribute("pid", "_"); //Prepares pid attribute in "standard position"
    node.AddAttribute("name", propertyToAdd.first);
    auto subNode = node.AddNewChild("lpwstr", "vt");
    subNode.AddContent(propertyToAdd.second);
  }

  // Fix pid attribute
  auto i = 2;
  for (auto node = rootNode.GetFirstChild(); !node.IsNull(); node = node.GetNextNode()) {
    node.AddAttribute("pid", std::to_string(i++));
  }
}

} // namespace file
} // namespace mip
