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

XmlDocument::XmlDocument(const string& xmlContent)
    : mXmlDoc(nullptr, DeleteXmlDoc) {
  mXmlDoc.reset(xmlParseMemory(xmlContent.c_str(), xmlContent.length()));
  if (!mXmlDoc)
    throw invalid_argument("xmlContent is not a valid XML");
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
  xmlDocDumpMemory(mXmlDoc.get(), &buf, &length);
  auto str = UniquePtr<xmlChar>(buf, DeleteXmlString);
  return ConvertXmlString(str.get());
}

XmlNode XmlDocument::GetRootNode() const {
  return XmlNode(xmlDocGetRootElement(mXmlDoc.get()));
}

}  // namespace mip
