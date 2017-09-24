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
  /* TODO: writer support
  void AppendChild(XmlNode newNode);
  XmlNode RemoveChild(XmlNode oldChild);*/
private:
  xmlNodePtr mNode;

  friend class XmlDocument;
};

} // namepsace mip

#endif // OPCFILEFORMAT_XML_XML_NODE_H_
