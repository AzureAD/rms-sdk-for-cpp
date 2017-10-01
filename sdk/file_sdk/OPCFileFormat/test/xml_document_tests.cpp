#include <memory>

#include "gtest/gtest.h"
#include "libxml/xmlmemory.h"

#include "OPCFileFormat/xml/xml_document.h"

using std::string;
using std::unique_ptr;

using namespace mip::file;

const string CUSTOM_XML =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
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
"</Properties>";

const string NODE_NAME = "nodeName";

TEST(XmlDocumentTests, ParseXmlDocument_InvalidXml_ThrowsInvalidArgument) {
  EXPECT_THROW(XmlDocument::ParseXmlDocument("Not an XML"), std::invalid_argument);
}

TEST(XmlDocumentTests, ParseXmlDocument_ValidXml_NotThrows) {
  EXPECT_NO_THROW(XmlDocument::ParseXmlDocument(CUSTOM_XML));
}

TEST(XmlDocumentTests, ParseXmlDocument_ValidXmlWithNewlinesAndTabs_ParseAndIgnoreWhitespaceNodes) {
  auto xml = XmlDocument::ParseXmlDocument("<root>\n\t<node>value</node>\n</root>");

  EXPECT_STREQ(R"(<?xml version="1.0" encoding="utf-8"?><root><node>value</node></root>)", xml.GetXmlContent().c_str());
}

TEST(XmlDocumentTests, CreateXmlDocument_CreateXml_CreatedXmlAsExpected) {
  auto doc = XmlDocument::CreateXmlDocument("root", "DefaultNamespaceUri", "NamespacePrefix", "NamespaceUri");
  EXPECT_STREQ(R"(<?xml version="1.0" encoding="utf-8"?><root xmlns:NamespacePrefix="NamespaceUri" xmlns="DefaultNamespaceUri"/>)", doc.GetXmlContent().c_str());
}

TEST(XmlDocumentTests, GetRootNode_CustomXml_RootNodeNameEqualsOuterMostNodeName) {
  const auto doc = XmlDocument::ParseXmlDocument(CUSTOM_XML);

  EXPECT_STREQ("Properties", doc.GetRootNode().GetNodeName().c_str());
}

TEST(XmlDocumentTests, SelectNode_InvalidXPath_NullNodeSelected) {
  const auto doc = XmlDocument::ParseXmlDocument(CUSTOM_XML);

  const auto node = doc.SelectNode("//*[@name='S");

  EXPECT_TRUE(node.IsNull());
}

TEST(XmlDocumentTests, SelectNode_ValidXPathWithoutResults_NullNodeSelected) {
  const auto doc = XmlDocument::ParseXmlDocument(CUSTOM_XML);

  const auto node = doc.SelectNode("//*[@name='NotExist']");

  EXPECT_TRUE(node.IsNull());
}

TEST(XmlDocumentTests, SelectNode_ValidXPathWithResults_SensitivityNodeSelected) {
  const auto doc = XmlDocument::ParseXmlDocument(CUSTOM_XML);

  const auto node = doc.SelectNode("//*[@name='Sensitivity']");

  EXPECT_STREQ("Sensitivity", node.GetAttributeValue("name").c_str());
}

void AddPropertyNode(XmlDocument& doc, const string& fmtid, const string& pid, const string& name, const string& value) {
  auto prop = doc.GetRootNode().AddNewChild("property");
  prop.AddAttribute("fmtid", fmtid);
  prop.AddAttribute("pid", pid);
  prop.AddAttribute("name", name);

  auto valueNode = prop.AddNewChild("lpwstr", "vt");
  valueNode.AddContent(value);
}

TEST(XmlDocumentTests, Integration_CreateCustomXmlFromScratch_XmlCreatedAsExpected) {
  auto content = CUSTOM_XML;
  auto doc = XmlDocument::CreateXmlDocument("Properties", "http://schemas.openxmlformats.org/officeDocument/2006/custom-properties", "vt", "http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes");
  AddPropertyNode(doc, "{D5CDD505-2E9C-101B-9397-08002B2CF9AE}", "2", "MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_Enabled", "True");
  AddPropertyNode(doc, "{D5CDD505-2E9C-101B-9397-08002B2CF9AE}", "3", "MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_Ref", "https://rmsibizaapidf.trafficmanager.net/api/72f988bf-86f1-41af-91ab-2d7cd011db47");
  AddPropertyNode(doc, "{D5CDD505-2E9C-101B-9397-08002B2CF9AE}", "4", "MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_AssignedBy", "user@microsoft.com");
  AddPropertyNode(doc, "{D5CDD505-2E9C-101B-9397-08002B2CF9AE}", "5", "MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_DateCreated", "2016-12-26T18:48:49.6421826+02:00");
  AddPropertyNode(doc, "{D5CDD505-2E9C-101B-9397-08002B2CF9AE}", "6", "MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_Name", "Label");
  AddPropertyNode(doc, "{D5CDD505-2E9C-101B-9397-08002B2CF9AE}", "7", "MSIP_Label_87867195-f2b8-4ac2-b0b6-8e099f4731fa_Extended_MSFT_Method", "Manual");
  AddPropertyNode(doc, "{D5CDD505-2E9C-101B-9397-08002B2CF9AE}", "8", "Sensitivity", "Label");

  EXPECT_STREQ(CUSTOM_XML.c_str(), doc.GetXmlContent().c_str());
}

TEST(XmlDocumentTests, Integration_RemoveAllMsipPropertiesFromCustomXml_XmlHasNoPropertiesLeft) {
  auto doc = XmlDocument::ParseXmlDocument(CUSTOM_XML);
  auto properties = doc.GetRootNode();
  auto property = properties.GetFirstChild();
  while (!(property == XmlNode())) {
    auto nextProperty = property.GetNextNode();
    auto name = property.GetAttributeValue("name");
    if (name.substr(0, 4) == "MSIP" || name == "Sensitivity")
      property.Delete();
    property = nextProperty;
  }

  EXPECT_STREQ(R"(<?xml version="1.0" encoding="utf-8"?><Properties xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes" xmlns="http://schemas.openxmlformats.org/officeDocument/2006/custom-properties"/>)", doc.GetXmlContent().c_str());
}