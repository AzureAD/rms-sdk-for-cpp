#ifndef OPCFILEFORMAT_XML_XML_DOCUMENT_H_
#define OPCFILEFORMAT_XML_XML_DOCUMENT_H_

#include <memory>
#include <string>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "libxml/tree.h"
#ifdef __cplusplus
}
#endif // __cplusplus

#include "OPCFileFormat/xml/xml_helper.h"
#include "OPCFileFormat/xml/xml_node.h"

namespace mip {

/**
 * \brief defines abstraction over XML document.
 * Derive from this class only for tests mocks.
 */
class XmlDocument {
public:
  explicit XmlDocument(const std::string& xmlContent);
  virtual XmlNode SelectNode(const std::string& xpath) const;

  virtual std::string GetXmlContent() const;

  virtual XmlNode GetRootNode() const;

private:
  UniquePtr<xmlDoc> mXmlDoc;

  /* TODO: writer support
  XmlNode CreateElement(std::string name, std::string namespace);
  void AppendAttribute(XmlNode node, std::string name, std::string value);*/
};

} // namepsace mip

#endif // OPCFILEFORMAT_XML_XML_DOCUMENT_H_
