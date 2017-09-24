#include <memory>

#include "gtest/gtest.h"
#include "libxml/xmlmemory.h"

#include "OPCFileFormat/xml/xml_document.h"

using std::string;
using std::unique_ptr;

using namespace mip;

const string xml =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<Properties xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\" xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/custom-properties\">"
  "<property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"2\" name=\"MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_Enabled\">"
    "<vt:lpwstr>True</vt:lpwstr>"
  "</property>"
  "<property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"3\" name=\"MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_Ref\">"
    "<vt:lpwstr>https://rmsibizaapidf.trafficmanager.net/api/72f988bf-86f1-41af-91ab-2d7cd011db47</vt:lpwstr>"
  "</property>"
  "<property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"4\" name=\"MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_AssignedBy\">"
    "<vt:lpwstr>user@microsoft.com</vt:lpwstr>"
  "</property>"
  "<property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"5\" name=\"MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_DateCreated\">"
    "<vt:lpwstr>2016-12-26T18:48:49.6421826+02:00</vt:lpwstr>"
  "</property>"
  "<property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"6\" name=\"MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_Name\">"
    "<vt:lpwstr>Label</vt:lpwstr>"
  "</property>"
  "<property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"7\" name=\"MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_Extended_MSFT_Method\">"
    "<vt:lpwstr>Manual</vt:lpwstr>"
  "</property>"
  "<property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"8\" name=\"Sensitivity\">"
    "<vt:lpwstr>Label</vt:lpwstr>"
  "</property>"
"</Properties>\n";

TEST(XmlDocumentTests, Ctor_InvalidXml_ThrowsInvalidArgument) {
  EXPECT_THROW(XmlDocument doc("Not an XML"), std::invalid_argument);
}

TEST(XmlDocumentTests, Ctor_ValidXml_NotThrows) {
  EXPECT_NO_THROW(XmlDocument doc(xml));
}

TEST(XmlDocumentTests, GetXmlContent_StringDestroyed_XmlContentStaysTheSame) {
  unique_ptr<const XmlDocument> doc;

  {
    const auto xmlCopy(xml);
    doc.reset(new XmlDocument(xmlCopy));
  }

  EXPECT_STREQ(xml.c_str(), doc->GetXmlContent().c_str());
}

TEST(XmlDocumentTests, GetRootNode_CustomXml_RootNodeNameEqualsOuterMostNodeName) {
  const XmlDocument doc(xml);

  EXPECT_STREQ("Properties", doc.GetRootNode().GetNodeName().c_str());
}

TEST(XmlDocumentTests, SelectNode_InvalidXPath_NullNodeSelected) {
  const XmlDocument doc(xml);
  const auto node = doc.SelectNode("//*[@name='S");
  EXPECT_EQ(XmlNode(), node);
}

TEST(XmlDocumentTests, SelectNode_ValidXPathWithoutResults_NullNodeSelected) {
  const XmlDocument doc(xml);
  const auto node = doc.SelectNode("//*[@name='NotExist']");
  EXPECT_EQ(XmlNode(), node);
}

TEST(XmlDocumentTests, SelectNode_ValidXPathWithResults_SensitivityNodeSelected) {
  const XmlDocument doc(xml);
  const auto node = doc.SelectNode("//*[@name='Sensitivity']");
  EXPECT_STREQ("Sensitivity", node.GetAttributeValue("name").c_str());
}