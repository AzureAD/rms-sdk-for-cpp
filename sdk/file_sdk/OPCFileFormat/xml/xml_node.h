#ifndef OPCFILEFORMAT_XML_XML_NODE_H_
#define OPCFILEFORMAT_XML_XML_NODE_H_

#include <string>
#include "libxml/tree.h"

namespace mip {
namespace file {

struct XmlNamespace {
  const std::string prefix;
  const std::string uri;
};

/**
* \brief defines abstraction over XML node.
* Derive from this class only for tests mocks.
*/
class XmlNode {
public:
  explicit XmlNode(xmlNodePtr node = nullptr) : mNode(node) { }

  virtual std::string GetAttributeValue(const std::string& attributeName) const;
  virtual XmlNode GetNextNode() const;
  virtual XmlNode GetFirstChild() const;
  virtual std::string GetNodeName() const;
  virtual std::string GetNodeInnerText() const;
  virtual XmlNamespace GetNodeNamespace() const;

  bool operator==(const XmlNode& rhs) const { return mNode == rhs.mNode; }
  bool IsNull() const { return mNode == nullptr; }

  virtual void AddAttribute(const std::string& attributeName, const std::string& attributeValue);
  virtual XmlNode AddNewChild(const std::string& name);
  virtual XmlNode AddNewChild(const std::string& name, const std::string& namespaceName);
  virtual void AddContent(const std::string& content);
  virtual void Delete();

private:
  XmlNode XmlNode::AddNewChildImpl(const std::string& name, xmlNsPtr ns);

  xmlNodePtr mNode;

  friend class XmlDocument;
};

} // namespace file
} // namepsace mip

#endif // OPCFILEFORMAT_XML_XML_NODE_H_
