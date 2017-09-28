#include "gtest/gtest.h"

#include "OPCFileFormat/xml/xml_document.h"

using std::string;

using namespace mip::file;

const string NODE_NAME = "nodeName";
const string ATTRIBUTE_NAME = "attributeName";
const string NAMESPACE_NAME = "namespaceName";
const string CONTENT_VALUE = "value";

TEST(XmlNodeTests, GetAttributeValue_NullNode_ReturnEmptyString) {
  EXPECT_STREQ("", XmlNode().GetAttributeValue(ATTRIBUTE_NAME).c_str());
}

TEST(XmlNodeTests, GetAttributeValue_NoAttributes_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_STREQ("", XmlNode(node.get()).GetAttributeValue(ATTRIBUTE_NAME).c_str());
}

TEST(XmlNodeTests, GetAttributeValue_NoAttributeWithGivenName_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlNewProp(node.get(), ConvertXmlString(ATTRIBUTE_NAME), ConvertXmlString(CONTENT_VALUE));

  EXPECT_STREQ("", XmlNode(node.get()).GetAttributeValue("noAttr").c_str());
}

TEST(XmlNodeTests, GetAttributeValue_NodeWithGivenAttributeName_ReturnAttributeValue) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlNewProp(node.get(), ConvertXmlString(ATTRIBUTE_NAME), ConvertXmlString(CONTENT_VALUE));

  EXPECT_STREQ(CONTENT_VALUE.c_str(), XmlNode(node.get()).GetAttributeValue(ATTRIBUTE_NAME).c_str());
}

TEST(XmlNodeTests, GetNextNode_NullNode_ReturnNullNode) {
  EXPECT_EQ(XmlNode(), XmlNode().GetNextNode());
}

TEST(XmlNodeTests, GetNextNode_NoNextNode_ReturnNullNode) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_EQ(XmlNode(), XmlNode(node.get()).GetNextNode());
}

TEST(XmlNodeTests, GetNextNode_HasNextNode_ReturnNextNode) {
  auto node1 = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "1")), xmlFreeNode);
  auto node2 = xmlAddNextSibling(node1.get(), xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "2")));

  EXPECT_EQ(XmlNode(node2), XmlNode(node1.get()).GetNextNode());
}

TEST(XmlNodeTests, GetFirstChild_NullNode_ReturnNullNode) {
  EXPECT_EQ(XmlNode(), XmlNode().GetFirstChild());
}

TEST(XmlNodeTests, GetFirstChild_NoChildren_ReturnNullNode) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_EQ(XmlNode(), XmlNode(node.get()).GetFirstChild());
}

TEST(XmlNodeTests, GetFirstChild_HasChildren_ReturnFirstChildNode) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto childNode = xmlAddChild(node.get(), xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "Child")));

  EXPECT_EQ(XmlNode(childNode), XmlNode(node.get()).GetFirstChild());
}

TEST(XmlNodeTests, GetNodeName_NullNode_ReturnEmptyString) {
  EXPECT_STREQ("", XmlNode().GetNodeName().c_str());
}

TEST(XmlNodeTests, GetNodeName_NoNodeName_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, nullptr), xmlFreeNode);

  EXPECT_STREQ("", XmlNode(node.get()).GetNodeName().c_str());
}

TEST(XmlNodeTests, GetNodeName_NodeWithName_ReturnNodeName) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_STREQ(NODE_NAME.c_str(), XmlNode(node.get()).GetNodeName().c_str());
}

TEST(XmlNodeTests, GetNodeInnerText_NullNode_ReturnEmptyString) {
  EXPECT_STREQ("", XmlNode().GetNodeInnerText().c_str());
}

TEST(XmlNodeTests, GetNodeInnerText_NoChildNode_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_STREQ("", XmlNode(node.get()).GetNodeInnerText().c_str());
}

TEST(XmlNodeTests, GetNodeInnerText_ChildNodeIsNotTextNode_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlAddChild(node.get(), xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "Child")));

  EXPECT_STREQ("", XmlNode(node.get()).GetNodeInnerText().c_str());
}

TEST(XmlNodeTests, GetNodeInnerText_NoChildNodeContent_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlAddChild(node.get(), xmlNewText(nullptr));

  EXPECT_STREQ("", XmlNode(node.get()).GetNodeInnerText().c_str());
}

TEST(XmlNodeTests, GetNodeInnerText_NodeWithChildTextNode_ReturnChildTextValue) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlAddChild(node.get(), xmlNewText(ConvertXmlString(CONTENT_VALUE)));

  EXPECT_STREQ(CONTENT_VALUE.c_str(), XmlNode(node.get()).GetNodeInnerText().c_str());
}

TEST(XmlNodeTests, GetNodeNamespace_NullNode_ReturnEmptyString) {
  EXPECT_STREQ("", XmlNode().GetNodeNamespace().c_str());
}

TEST(XmlNodeTests, GetNodeNamespace_NodeWithoutNamespace_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_STREQ("", XmlNode(node.get()).GetNodeNamespace().c_str());
}

TEST(XmlNodeTests, GetNodeNamespaceNodeWithNamespace_ReturnNodeNamespace) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  const auto ns = xmlNewNs(node.get(), ConvertXmlString("http://schemas.openxmlformats.org/officeDocument/2006/custom-properties"), ConvertXmlString(NAMESPACE_NAME));
  xmlSetNs(node.get(), ns);

  EXPECT_STREQ(NAMESPACE_NAME.c_str(), XmlNode(node.get()).GetNodeNamespace().c_str());
}

TEST(XmlNodeTests, AddAttribute_NullNode_DoesNotThrow) {
  EXPECT_NO_THROW(XmlNode().AddAttribute("",""));
}

TEST(XmlNodeTests, AddAttribute_ValidAttribute_AttributeAdded) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  XmlNode xmlNode(node.get());
  xmlNode.AddAttribute(ATTRIBUTE_NAME, CONTENT_VALUE);

  EXPECT_STREQ(CONTENT_VALUE.c_str(), xmlNode.GetAttributeValue(ATTRIBUTE_NAME).c_str());
}

TEST(XmlNodeTests, AddChild_NullParentAndChildNode_DoesNotThrow) {
  EXPECT_NO_THROW(XmlNode().AddChild(XmlNode()));
}

TEST(XmlNodeTests, AddChild_ValidChildNode_ChildAdded) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto xmlNode = XmlNode(node.get());
  auto xmlChildNode = XmlNode(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "Child")));

  xmlNode.AddChild(xmlChildNode);

  ASSERT_EQ(xmlChildNode, xmlNode.GetFirstChild());
}

TEST(XmlNodeTests, AddContent_NullNode_DoesNotThrow) {
  EXPECT_NO_THROW(XmlNode().AddContent(CONTENT_VALUE));
}

TEST(XmlNodeTests, AddContent_ValidContent_ContentAdded) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto xmlNode = XmlNode(node.get());

  xmlNode.AddContent(CONTENT_VALUE);

  EXPECT_STREQ(CONTENT_VALUE.c_str(), xmlNode.GetNodeInnerText().c_str());
}

TEST(XmlNodeTests, AddContent_ContentWithSpecialChars_ContentAdded) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto xmlNode = XmlNode(node.get());

  auto content = CONTENT_VALUE + "&<>";
  xmlNode.AddContent(content);

  EXPECT_STREQ(content.c_str(), xmlNode.GetNodeInnerText().c_str());
}

TEST(XmlNodeTests, Delete_NullNode_DoesNotThrow) {
  EXPECT_NO_THROW(XmlNode().Delete());
}

TEST(XmlNodeTests, Delete_ValidNode_NodeIsNullAfterDelete) {
  auto xmlNode = XmlNode(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)));

  xmlNode.Delete();

  EXPECT_EQ(XmlNode(), xmlNode);
}

TEST(XmlNodeTests, Delete_DeleteChildNode_ParentDoesNotHaveChildren) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto xmlNode = XmlNode(node.get());
  auto xmlChildNode = XmlNode(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "Child")));

  xmlNode.AddChild(xmlChildNode);
  xmlChildNode.Delete();

  ASSERT_EQ(XmlNode(), xmlNode.GetFirstChild());
}
