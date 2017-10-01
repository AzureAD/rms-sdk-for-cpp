#include "OPCFileFormat/xml/xml_node.h"
#include "libxml/globals.h"
#include "OPCFileFormat/xml/xml_helper.h"

using std::string;

namespace mip {
namespace file {

string XmlNode::GetAttributeValue(const string& attributeName) const {
  if (!mNode)
    return string();

  const auto attributeValue = UniquePtr<xmlChar>(xmlGetProp(mNode, ConvertXmlString(attributeName)), DeleteXmlString);
  return attributeValue ? ConvertXmlString(attributeValue.get()) : string();
}

XmlNode XmlNode::GetNextNode() const {
  return XmlNode(mNode ? mNode->next : nullptr);
}

XmlNode XmlNode::GetFirstChild() const {
  return XmlNode(mNode ? mNode->children : nullptr);
}

string XmlNode::GetNodeName() const {
  if (mNode && mNode->name)
    return ConvertXmlString(mNode->name);
  return string();
}

string XmlNode::GetNodeInnerText() const {
  if (mNode && mNode->children && mNode->children->type == XML_TEXT_NODE && mNode->children->content)
    return ConvertXmlString(mNode->children->content);
  return string();
}

string XmlNode::GetNodeNamespace() const {
  if (mNode && mNode->ns && mNode->ns->prefix)
    return ConvertXmlString(mNode->ns->prefix);
  return string();
}

void XmlNode::AddAttribute(const string& attributeName, const string& attributeValue) {
  if (!mNode)
    return;
  xmlSetProp(mNode, ConvertXmlString(attributeName), ConvertXmlString(attributeValue));
}

XmlNode XmlNode::AddNewChild(const string& name) {
  if (!mNode || !mNode->doc)
    return XmlNode();
  return AddNewChildImpl(name, nullptr);
}

XmlNode XmlNode::AddNewChild(const string& name, const string& namespaceName) {
  if (!mNode || !mNode->doc)
    return XmlNode();
  const auto ns = xmlSearchNs(mNode->doc, xmlDocGetRootElement(mNode->doc), ConvertXmlString(namespaceName));
  return AddNewChildImpl(name, ns);
}

XmlNode XmlNode::AddNewChildImpl(const string& name, xmlNsPtr ns) {
  XmlNode newChild(xmlNewDocNode(mNode->doc, ns, ConvertXmlString(name), nullptr));
  xmlAddChild(mNode, newChild.mNode);
  return newChild;
}

void XmlNode::AddContent(const string& content) {
  if (!mNode)
    return;
  xmlAddChild(mNode, xmlNewText(ConvertXmlString(content)));
}

void XmlNode::Delete() {
  if (!mNode)
    return;
  xmlUnlinkNode(mNode);
  xmlFreeNode(mNode);
  mNode = nullptr;
}

} // namespace file
}  // namespace mip
