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
  static XmlDocument CreateXmlDocument(const std::string& rootNodeName, const std::string& defaultNamespaceUri, const std::string& namespacePrefix, const std::string& namespaceUri);
  static XmlDocument ParseXmlDocument(const std::string& xmlContent);

  virtual XmlNode SelectNode(const std::string& xpath) const;
  virtual std::string GetXmlContent() const;
  virtual XmlNode GetRootNode() const;

  virtual XmlNode CreateNode(const std::string& name);
  virtual XmlNode CreateNode(const std::string& name, const std::string& namespaceName);

protected:
  XmlDocument();

private:
  UniquePtr<xmlDoc> mXmlDoc;

};

} // namepsace mip

#endif // OPCFILEFORMAT_XML_XML_DOCUMENT_H_
