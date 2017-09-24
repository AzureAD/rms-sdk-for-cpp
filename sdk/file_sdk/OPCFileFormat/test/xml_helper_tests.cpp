#include "gtest/gtest.h"
#include "libxml/tree.h"

#include "OPCFileFormat/xml/xml_helper.h"

using std::string;

using namespace mip;

TEST(XmlHelperTests, DeleteXmlString_DeleteNull_DeleteNotFailed) {
  DeleteXmlString(nullptr);
}

TEST(XmlHelperTests, DeleteXmlString_DeleteAllocatedXmlString_XmlStringFreed) {
  string s = "text";
  xmlChar* xmlStr = xmlCharStrdup(s.c_str());

  DeleteXmlString(xmlStr);

  EXPECT_NE('t', xmlStr[0]);
}

TEST(XmlHelperTests, ConvertXmlString_ConvertStdString_ReturnedPointerEqualsStdCstring) {
  string s("text");
  auto cstring = static_cast<const void*>(s.c_str());

  EXPECT_EQ(cstring, ConvertXmlString(s));
}

TEST(XmlHelperTests, ConvertXmlString_ConvertNullXmlString_ReturnEmptyString) {
  EXPECT_EQ(string(), ConvertXmlString(nullptr));
}

TEST(XmlHelperTests, ConvertXmlString_ConvertValidXmlString_ReturnedStringEqualsXmlString) {
  string s = "text";
  const xmlChar xmlStr[5] = "text";

  EXPECT_TRUE(s == ConvertXmlString(xmlStr));
}

TEST(XmlHelperTests, DeleteXmlDoc_DeleteNull_DeleteNotFailed) {
  DeleteXmlDoc(nullptr);
}

TEST(XmlHelperTests, DeleteXmlDoc_DeleteAllocatedXmlDocAndItsRootNode_XmlDocAndNodeAreFreed) {
  xmlDocPtr doc = xmlNewDoc(nullptr);
  xmlNodePtr root = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("root"));
  xmlDocSetRootElement(doc, root);

  EXPECT_EQ(XML_DOCUMENT_NODE, doc->type);
  EXPECT_EQ(XML_ELEMENT_NODE, root->type);

  DeleteXmlDoc(doc);

  EXPECT_NE(XML_DOCUMENT_NODE, doc->type);
  EXPECT_NE(XML_ELEMENT_NODE, root->type);
}
