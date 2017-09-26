#include <string>
#include "gtest/gtest.h"
#include "OPCFileFormat/xml/custom_properties.h"

using std::string; 
const string emptyXml = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/custom-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes" />)";

TEST(CustomProperties, EmptyProperties_EmptyTags) {
  auto customProperties = mip::file::CustomPropertiesXml{ emptyXml };
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(0, tags.size());
}

const string singlePropertyXml = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/custom-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">
  <property fmtid="{D5CDD505-2E9C-101B-9397-08002B2CF9AE}" pid="1" name="PropName">
    <vt:lpwstr>PropValue</vt:lpwstr>
  </property>
</Properties>)";

TEST(CustomProperties, SingleProperty_SingleTag) {
  auto customProperties = mip::file::CustomPropertiesXml{ singlePropertyXml };
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(1, tags.size());
  EXPECT_EQ(tags[0], mip::file::CustomProperty("PropName", "PropValue"));
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
TEST(CustomProperties, MultipleProperties_MultipleTags) {
  auto customProperties = mip::file::CustomPropertiesXml{ multiPropertyXml };
  auto tags = customProperties.GetProperties();
  EXPECT_EQ(3, tags.size());
  EXPECT_EQ(tags[0], mip::file::CustomProperty("PropName0", "PropValue0"));
  EXPECT_EQ(tags[1], mip::file::CustomProperty("PropName1", "PropValue1"));
  EXPECT_EQ(tags[2], mip::file::CustomProperty("PropName2", "PropValue2"));
}
