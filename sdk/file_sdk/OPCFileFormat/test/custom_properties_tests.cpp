#include <string>
#include "gtest/gtest.h"
#include "OPCFileFormat/xml/custom_properties.h"

using std::string;
using std::vector;
using namespace mip::file;

const string emptyXml = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/custom-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes" />)";

TEST(CustomProperties, GetProperties_EmptyProperties_EmptyTags) {
  CustomPropertiesXml customProperties { emptyXml };
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(0, tags.size());
}

const string singlePropertyXml = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/custom-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="1" name="PropName">
    <vt:lpwstr>PropValue</vt:lpwstr>
  </property>
</Properties>)";

TEST(CustomProperties, GetProperties_SingleProperty_SingleTag) {
  CustomPropertiesXml customProperties{ singlePropertyXml };
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(1, tags.size());
  EXPECT_EQ(tags[0], CustomProperty("PropName", "PropValue"));
}

const string multiPropertyXml = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/custom-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="1" name="PropName0">
    <vt:lpwstr>PropValue0</vt:lpwstr>
  </property>
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="2" name="PropName1">
    <vt:lpwstr>PropValue1</vt:lpwstr>
  </property>
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="3" name="PropName2">
    <vt:lpwstr>PropValue2</vt:lpwstr>
  </property>
</Properties>)";
TEST(CustomProperties, GetProperties_MultipleProperties_MultipleTags) {
  CustomPropertiesXml customProperties{ multiPropertyXml };
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(3, tags.size());
  EXPECT_EQ(tags[0], CustomProperty("PropName0", "PropValue0"));
  EXPECT_EQ(tags[1], CustomProperty("PropName1", "PropValue1"));
  EXPECT_EQ(tags[2], CustomProperty("PropName2", "PropValue2"));
}

const string wrongPropertyNodeXml = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/custom-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="1" name="PropName0">
    <vt:lpwstr>PropValue0</vt:lpwstr>
  </property>
  <wrong fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="2" name="PropName1">
    <vt:lpwstr>PropValue1</vt:lpwstr>
  </wrong>
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="3" name="PropName2">
    <vt:i4>2</vt:i4>
  </property>
</Properties>)";
TEST(CustomProperties, GetProperties_WrongPropertyNode_NodeSkipped) {
  CustomPropertiesXml customProperties{ wrongPropertyNodeXml };
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(2, tags.size());
  EXPECT_EQ(tags[0], CustomProperty("PropName0", "PropValue0"));
  EXPECT_EQ(tags[1], CustomProperty("PropName2", "2"));
}

const string wrongPropertySubNodeXml = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/custom-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="1" name="PropName0">
    <vt:lpwstr>PropValue0</vt:lpwstr>
  </property>
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="2" name="PropName1">
    <vt1:lpwstr>PropValue1</vt1:lpwstr>
  </property>
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="3" name="PropName2">
    <vt:i4>2</vt:i4>
  </property>
</Properties>)";
TEST(CustomProperties, GetProperties_WrongPropertySubNode_NodeSkipped) {
  CustomPropertiesXml customProperties{ wrongPropertySubNodeXml };
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(2, tags.size());
  EXPECT_EQ(tags[0], CustomProperty("PropName0", "PropValue0"));
  EXPECT_EQ(tags[1], CustomProperty("PropName2", "2"));
}

TEST(CustomProperties, UpdateProperties_Delete_MultipleProperties_MultipleTags) {
  CustomPropertiesXml customProperties{ multiPropertyXml };
  vector<string> del{ "PropName0", "PropName2" };
  vector<CustomProperty> vec;
  customProperties.UpdateProperties(vec, del);
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(1, tags.size());
}

TEST(CustomProperties, UpdateProperties_Delete_SamePropertyMultipleTimes_DoesntFail) {
  CustomPropertiesXml customProperties{ multiPropertyXml };
  vector<string> del{ "PropName0", "PropName0" };
  vector<CustomProperty> vec;
  customProperties.UpdateProperties(vec, del);
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(2, tags.size());
}

TEST(CustomProperties, UpdateProperties_Delete_WrongProperty_DoesntFail) {
  CustomPropertiesXml customProperties{ multiPropertyXml };
  vector<string> del{ "PropNameX", "PropNameY" };
  vector<CustomProperty> vec;
  customProperties.UpdateProperties(vec, del);
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(3, tags.size());
}

TEST(CustomProperties, UpdateProperties_Add_MultipleProperties_MultipleTags) {
  CustomPropertiesXml customProperties{ multiPropertyXml };
  vector<string> del;
  vector<CustomProperty> vec { CustomProperty("PropName4", "PropValue4") , CustomProperty("PropName5", "PropValue5") };
  customProperties.UpdateProperties(vec, del);
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(5, tags.size());
  EXPECT_NE(tags.end(), std::find(tags.begin(), tags.end(), CustomProperty("PropName4", "PropValue4")));
  EXPECT_NE(tags.end(), std::find(tags.begin(), tags.end(), CustomProperty("PropName5", "PropValue5")));
}

TEST(CustomProperties, UpdateProperties_MultipleProperties_MultipleTags) {
  CustomPropertiesXml customProperties{ multiPropertyXml };
  vector<string> del{ "PropName0", "PropName2" };
  vector<CustomProperty> vec{ CustomProperty("PropName4", "PropValue4") , CustomProperty("PropName5", "PropValue5") };
  customProperties.UpdateProperties(vec, del);
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(3, tags.size());
  EXPECT_NE(tags.end(), std::find(tags.begin(), tags.end(), CustomProperty("PropName1", "PropValue1")));
  EXPECT_NE(tags.end(), std::find(tags.begin(), tags.end(), CustomProperty("PropName4", "PropValue4")));
  EXPECT_NE(tags.end(), std::find(tags.begin(), tags.end(), CustomProperty("PropName5", "PropValue5")));
}

const string expectdXml =
R"(<?xml version="1.0" encoding="utf-8" standalone="yes"?>)"
R"(<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/custom-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">)"
R"(<property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="2" name="PropName1">)"
R"(<vt:lpwstr>PropValue1</vt:lpwstr>)"
R"(</property>)"
R"(<property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="3" name="PropName4">)"
R"(<vt:lpwstr>PropValue4</vt:lpwstr>)"
R"(</property>)"
R"(<property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="4" name="PropName5">)"
R"(<vt:lpwstr>PropValue5</vt:lpwstr>)"
R"(</property>)"
R"(</Properties>)";


TEST(CustomProperties, UpdateProperties_Serialize_StringUpdated) {
  CustomPropertiesXml customProperties{ multiPropertyXml };
  vector<string> del{ "PropName0", "PropName2" };
  vector<CustomProperty> vec{ CustomProperty("PropName4", "PropValue4") , CustomProperty("PropName5", "PropValue5") };
  customProperties.UpdateProperties(vec, del);
  EXPECT_STREQ(expectdXml.c_str(), customProperties.Serialize().c_str());
}
