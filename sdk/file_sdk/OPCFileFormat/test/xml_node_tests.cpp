#include "gtest/gtest.h"

#include "OPCFileFormat/xml/xml_node.h"
#include "OPCFileFormat/xml/xml_helper.h"

using std::string;

using namespace mip::file;

const string NODE_NAME = "nodeName";
const string CHILD_NODE_NAME = "nodeNameChild";
const string ATTRIBUTE_NAME = "attributeName";
const string NAMESPACE_NAME = "namespaceName";
const string CONTENT_VALUE = "value";
const string NAMESPACE_URL = "NamespaceURL";

TEST(XmlNodeTests, GetAttributeValue_NullNode_ReturnEmptyString) {
  EXPECT_EQ("", XmlNode().GetAttributeValue(ATTRIBUTE_NAME));
}

TEST(XmlNodeTests, GetAttributeValue_NoAttributes_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_EQ("", XmlNode(node.get()).GetAttributeValue(ATTRIBUTE_NAME));
}

TEST(XmlNodeTests, GetAttributeValue_NoAttributeWithGivenName_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlNewProp(node.get(), ConvertXmlString(ATTRIBUTE_NAME), ConvertXmlString(CONTENT_VALUE));

  EXPECT_EQ("", XmlNode(node.get()).GetAttributeValue("noAttr"));
}

TEST(XmlNodeTests, GetAttributeValue_NodeWithGivenAttributeName_ReturnAttributeValue) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlNewProp(node.get(), ConvertXmlString(ATTRIBUTE_NAME), ConvertXmlString(CONTENT_VALUE));

  EXPECT_EQ(CONTENT_VALUE, XmlNode(node.get()).GetAttributeValue(ATTRIBUTE_NAME));
}

TEST(XmlNodeTests, GetNextNode_NullNode_ReturnNullNode) {
  EXPECT_TRUE(XmlNode().GetNextNode().IsNull());
}

TEST(XmlNodeTests, GetNextNode_NoNextNode_ReturnNullNode) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_TRUE(XmlNode(node.get()).GetNextNode().IsNull());
}

TEST(XmlNodeTests, GetNextNode_HasNextNode_ReturnNextNode) {
  auto node1 = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "1")), xmlFreeNode);
  auto node2 = xmlAddNextSibling(node1.get(), xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "2")));

  EXPECT_EQ(XmlNode(node2), XmlNode(node1.get()).GetNextNode());
}

TEST(XmlNodeTests, GetFirstChild_NullNode_ReturnNullNode) {
  EXPECT_TRUE(XmlNode().GetFirstChild().IsNull());
}

TEST(XmlNodeTests, GetFirstChild_NoChildren_ReturnNullNode) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_TRUE(XmlNode(node.get()).GetFirstChild().IsNull());
}

TEST(XmlNodeTests, GetFirstChild_HasChildren_ReturnFirstChildNode) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto childNode = xmlAddChild(node.get(), xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "Child")));

  EXPECT_EQ(XmlNode(childNode), XmlNode(node.get()).GetFirstChild());
}

TEST(XmlNodeTests, GetNodeName_NullNode_ReturnEmptyString) {
  EXPECT_EQ("", XmlNode().GetNodeName());
}

TEST(XmlNodeTests, GetNodeName_NoNodeName_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, nullptr), xmlFreeNode);

  EXPECT_EQ("", XmlNode(node.get()).GetNodeName());
}

TEST(XmlNodeTests, GetNodeName_NodeWithName_ReturnNodeName) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_EQ(NODE_NAME, XmlNode(node.get()).GetNodeName());
}

TEST(XmlNodeTests, GetNodeInnerText_NullNode_ReturnEmptyString) {
  EXPECT_EQ("", XmlNode().GetNodeInnerText());
}

TEST(XmlNodeTests, GetNodeInnerText_NoChildNode_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  EXPECT_EQ("", XmlNode(node.get()).GetNodeInnerText());
}

TEST(XmlNodeTests, GetNodeInnerText_ChildNodeIsNotTextNode_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlAddChild(node.get(), xmlNewNode(nullptr, ConvertXmlString(NODE_NAME + "Child")));

  EXPECT_EQ("", XmlNode(node.get()).GetNodeInnerText());
}

TEST(XmlNodeTests, GetNodeInnerText_NoChildNodeContent_ReturnEmptyString) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlAddChild(node.get(), xmlNewText(nullptr));

  EXPECT_EQ("", XmlNode(node.get()).GetNodeInnerText());
}

TEST(XmlNodeTests, GetNodeInnerText_NodeWithChildTextNode_ReturnChildTextValue) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  xmlAddChild(node.get(), xmlNewText(ConvertXmlString(CONTENT_VALUE)));

  EXPECT_EQ(CONTENT_VALUE, XmlNode(node.get()).GetNodeInnerText());
}

void TestNamespace(const XmlNamespace& expected, const XmlNamespace& actual) {
  EXPECT_EQ(expected.prefix, actual.prefix);
  EXPECT_EQ(expected.uri, actual.uri);
}

TEST(XmlNodeTests, GetNodeNamespace_NullNode_ReturnEmptyNamespace) {
  TestNamespace(XmlNamespace(), XmlNode().GetNodeNamespace());
}

TEST(XmlNodeTests, GetNodeNamespace_NodeWithoutNamespace_ReturnEmptyNamespace) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);

  TestNamespace(XmlNamespace(), XmlNode(node.get()).GetNodeNamespace());
}

TEST(XmlNodeTests, GetNodeNamespace_NodeWithNamespace_ReturnNodeNamespace) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  XmlNamespace xmlNamespace { NAMESPACE_NAME, NAMESPACE_URL };
  const auto ns = xmlNewNs(node.get(), ConvertXmlString(xmlNamespace.uri), ConvertXmlString(xmlNamespace.prefix));
  xmlSetNs(node.get(), ns);

  TestNamespace(xmlNamespace, XmlNode(node.get()).GetNodeNamespace());
}

TEST(XmlNodeTests, GetNodeNamespace_NodeWithoutNamespaceInDocWithoutDefaultNamespace_ReturnEmptyNamespace) {
  UniquePtr<xmlDoc> doc(xmlNewDoc(nullptr), xmlFreeDoc);
  auto root = xmlNewNode(nullptr, ConvertXmlString(NODE_NAME));
  xmlDocSetRootElement(doc.get(), root);

  auto child = xmlNewDocNode(doc.get(), nullptr, ConvertXmlString(CHILD_NODE_NAME), nullptr);
  XmlNode childNode(child);
  xmlAddChild(root, child);

  EXPECT_EQ(nullptr, child->ns);
  TestNamespace(XmlNamespace(), childNode.GetNodeNamespace());
}

TEST(XmlNodeTests, GetNodeNamespace_NodeWithoutNamespaceInDocWithDefaultNamespace_ReturnDocDefaultNamespace) {
  UniquePtr<xmlDoc> doc(xmlNewDoc(nullptr), xmlFreeDoc);
  auto root = xmlNewNode(nullptr, ConvertXmlString(NODE_NAME));
  xmlDocSetRootElement(doc.get(), root);

  XmlNamespace defaultNamespace { string(), NAMESPACE_URL };
  xmlNewNs(root, ConvertXmlString(defaultNamespace.uri), nullptr);

  auto child = xmlNewDocNode(doc.get(), nullptr, ConvertXmlString(CHILD_NODE_NAME), nullptr);
  XmlNode childNode(child);
  xmlAddChild(root, child);

  EXPECT_EQ(nullptr, child->ns);
  TestNamespace(defaultNamespace, childNode.GetNodeNamespace());
}

TEST(XmlNodeTests, AddAttribute_NullNode_DoesNotThrow) {
  EXPECT_NO_THROW(XmlNode().AddAttribute("", ""));
}

TEST(XmlNodeTests, AddAttribute_ValidAttribute_AttributeAdded) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  XmlNode xmlNode(node.get());
  xmlNode.AddAttribute(ATTRIBUTE_NAME, CONTENT_VALUE);

  EXPECT_EQ(CONTENT_VALUE, xmlNode.GetAttributeValue(ATTRIBUTE_NAME));
}

TEST(XmlNodeTests, AddNewChild_NullParentAndEmptyChildName_DoesNotThrowAndReturnsNullChild) {
  EXPECT_NO_THROW(XmlNode().AddNewChild(""));
}

TEST(XmlNodeTests, AddNewChild_NullParentAndEmptyChildNameAndNamesace_DoesNotThrowAndReturnsNullChild) {
  EXPECT_NO_THROW(XmlNode().AddNewChild("", ""));
}

TEST(XmlNodeTests, AddNewChild_NullParentAndEmptyChildName_ReturnsNullChild) {
  EXPECT_TRUE(XmlNode().AddNewChild("").IsNull());
}

TEST(XmlNodeTests, AddNewChild_NullParentAndEmptyChildNameAndNamesace_ReturnsNullChild) {
  EXPECT_TRUE(XmlNode().AddNewChild("", "").IsNull());
}

TEST(XmlNodeTests, AddNewChild_NodeWithoutDoc_ChildNotAddedAndReturnsNullChild) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto xmlNode = XmlNode(node.get());

  auto xmlChildNode = xmlNode.AddNewChild(CHILD_NODE_NAME);

  EXPECT_TRUE(xmlNode.GetFirstChild().IsNull());
  EXPECT_TRUE(xmlChildNode.IsNull());
}

void TestAddNewChild(const char* namespaceName) {
  UniquePtr<xmlDoc> doc(xmlNewDoc(nullptr), xmlFreeDoc);
  auto root = xmlNewNode(nullptr, ConvertXmlString(NODE_NAME));
  XmlNode rootNode(root);
  xmlDocSetRootElement(doc.get(), root);
  XmlNamespace xmlNamespace { NAMESPACE_NAME, NAMESPACE_URL };
  xmlNewNs(root, ConvertXmlString(xmlNamespace.uri), ConvertXmlString(xmlNamespace.prefix));
  
  XmlNode childNode;
  if (namespaceName)
    childNode = rootNode.AddNewChild(NODE_NAME, namespaceName);
  else
    childNode = rootNode.AddNewChild(NODE_NAME);

  EXPECT_EQ(NODE_NAME, childNode.GetNodeName());
  EXPECT_EQ(childNode, rootNode.GetFirstChild());

  if (namespaceName && NAMESPACE_NAME == namespaceName)
    TestNamespace(xmlNamespace, childNode.GetNodeNamespace());
  else
    EXPECT_TRUE(childNode.GetNodeNamespace().uri.empty());
}

TEST(XmlDocumentTests, AddNewChild_NodeWithoutNamepsace_NodeCreatedWithoutNamespace) {
  TestAddNewChild(nullptr);
}

TEST(XmlDocumentTests, AddNewChild_NamespaceNotInDocument_NodeCreatedWithoutNamespace) {
  TestAddNewChild("nonamespace");
}

TEST(XmlDocumentTests, AddNewChild_EmptyNamespace_NodeCreatedWithoutNamespace) {
  TestAddNewChild("");
}

TEST(XmlDocumentTests, AddNewChild_ValidNamespace_NodeCreatedAndAdded) {
  TestAddNewChild(NAMESPACE_NAME.c_str());
}

TEST(XmlNodeTests, AddContent_NullNode_DoesNotThrow) {
  EXPECT_NO_THROW(XmlNode().AddContent(CONTENT_VALUE));
}

TEST(XmlNodeTests, AddContent_ValidContent_ContentAdded) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto xmlNode = XmlNode(node.get());

  xmlNode.AddContent(CONTENT_VALUE);

  EXPECT_EQ(CONTENT_VALUE, xmlNode.GetNodeInnerText());
}

TEST(XmlNodeTests, AddContent_ContentWithSpecialChars_ContentAdded) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto xmlNode = XmlNode(node.get());

  auto content = CONTENT_VALUE + "&<>";
  xmlNode.AddContent(content);

  EXPECT_EQ(content, xmlNode.GetNodeInnerText());
}

TEST(XmlNodeTests, Delete_NullNode_DoesNotThrow) {
  EXPECT_NO_THROW(XmlNode().Delete());
}

TEST(XmlNodeTests, Delete_ValidNode_NodeIsNullAfterDelete) {
  auto xmlNode = XmlNode(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)));

  xmlNode.Delete();

  EXPECT_TRUE(xmlNode.IsNull());
}

TEST(XmlNodeTests, Delete_DeleteChildNode_ParentDoesNotHaveChildren) {
  auto node = UniquePtr<xmlNode>(xmlNewNode(nullptr, ConvertXmlString(NODE_NAME)), xmlFreeNode);
  auto xmlNode = XmlNode(node.get());
  auto xmlChildNode = xmlNode.AddNewChild(CHILD_NODE_NAME);

  xmlChildNode.Delete();

  EXPECT_TRUE(xmlNode.GetFirstChild().IsNull());
}
