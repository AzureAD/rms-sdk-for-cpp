#ifndef OPCFILEFORMAT_XML_XML_NODE_H_
#define OPCFILEFORMAT_XML_XML_NODE_H_

#include <string>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "libxml/tree.h"
#ifdef __cplusplus
}
#endif // __cplusplus

namespace mip {
namespace file {

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
  virtual std::string GetNodeNamespace() const;

  bool operator==(const XmlNode& rhs) const { return mNode == rhs.mNode; }

  virtual void AddAttribute(const std::string& attributeName, const std::string& attributeValue);
  virtual void AddChild(XmlNode newNode);
  virtual void AddContent(const std::string& content);
  virtual void Delete();
private:
  xmlNodePtr mNode;

  friend class XmlDocument;
};

} // namespace file
} // namepsace mip

#endif // OPCFILEFORMAT_XML_XML_NODE_H_
