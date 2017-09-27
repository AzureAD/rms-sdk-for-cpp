#include "OPCFileFormat/xml/xml_document.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "libxml/parser.h"
#include "libxml/xpath.h"
#ifdef __cplusplus
}
#endif // __cplusplus

using std::string;
using std::invalid_argument;

namespace mip {
namespace file {

XmlDocument XmlDocument::CreateXmlDocument(const string& rootNodeName,
    const string& defaultNamespaceUri,
    const string& namespacePrefix,
    const string& namespaceUri) {
  XmlDocument xmlDoc;
  xmlDoc.mXmlDoc.reset(xmlNewDoc(nullptr));
  auto root = xmlNewNode(nullptr, ConvertXmlString(rootNodeName));
  xmlDocSetRootElement(xmlDoc.mXmlDoc.get(), root);
  xmlNewNs(root, ConvertXmlString(namespaceUri), ConvertXmlString(namespacePrefix));
  xmlNewNs(root, ConvertXmlString(defaultNamespaceUri), nullptr);
  return xmlDoc;
}

XmlDocument XmlDocument::ParseXmlDocument(const string& xmlContent) {
  XmlDocument xmlDoc;
  xmlDoc.mXmlDoc.reset(xmlParseMemory(xmlContent.c_str(), xmlContent.length()));
  if (!xmlDoc.mXmlDoc)
    throw invalid_argument("xmlContent is not a valid XML");
  return xmlDoc;
}

XmlDocument::XmlDocument() : mXmlDoc(nullptr, DeleteXmlDoc) {
}

XmlNode XmlDocument::SelectNode(const string& xpath) const {
  const auto context = UniquePtr<xmlXPathContext>(xmlXPathNewContext(mXmlDoc.get()), xmlXPathFreeContext);
  if (!context) {
    printf("Error in xmlXPathNewContext\n");
    return XmlNode();
  }

  const auto result = UniquePtr<xmlXPathObject>(xmlXPathEvalExpression(ConvertXmlString(xpath), context.get()), xmlXPathFreeObject);
  if (!result) {
    printf("Error in xmlXPathEvalExpression\n");
    return XmlNode();
  }

  if (!result->nodesetval || result->nodesetval->nodeNr == 0 || !result->nodesetval->nodeTab) {
    printf("No result\n");
    return XmlNode();
  }

  return XmlNode(result->nodesetval->nodeTab[0]);
}

string XmlDocument::GetXmlContent() const {
  int length;
  xmlChar * buf;
  xmlDocDumpMemoryEnc(mXmlDoc.get(), &buf, &length, "utf-8");
  auto str = UniquePtr<xmlChar>(buf, DeleteXmlString);
  auto content = ConvertXmlString(str.get());
  if (content.back() == '\n')
    content.pop_back();
  auto pos = content.find("?>\n");
  if (pos != string::npos)
    content.replace(pos, 3, "?>");
  return content;
}

XmlNode XmlDocument::GetRootNode() const {
  return XmlNode(xmlDocGetRootElement(mXmlDoc.get()));
}

XmlNode XmlDocument::CreateNode(const string& name) {
  return XmlNode(xmlNewDocNode(mXmlDoc.get(), nullptr, ConvertXmlString(name), nullptr));
}

XmlNode XmlDocument::CreateNode(const string& name, const string& namespaceName) {
  const auto ns = xmlSearchNs(mXmlDoc.get(), GetRootNode().mNode, ConvertXmlString(namespaceName));
  return XmlNode(xmlNewDocNode(mXmlDoc.get(), ns, ConvertXmlString(name), nullptr));
}

} // namespace file
} // namespace mip
