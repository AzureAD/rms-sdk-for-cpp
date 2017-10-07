#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <fstream>

#include "Common/istream.h"
#include "OPCFileFormat/zip_file.h"
#include "UnitTests/gtest/test_utils.h"

namespace mip {
namespace file {

using std::shared_ptr;
using std::string;

TEST(ZipTests, GetEntry_ReturnsXml) {
  shared_ptr<IStream> stream = GetIStreamFromFile("LabeledGeneral.docx");
  ZipFile zipFile(stream);
  string xml = zipFile.GetEntry("docProps/custom.xml");
  static auto expectedXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/custom-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\"><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"2\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Enabled\"><vt:lpwstr>True</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"3\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_SiteId\"><vt:lpwstr>72f988bf-86f1-41af-91ab-2d7cd011db47</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"4\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Ref\"><vt:lpwstr>https://api.informationprotection.azure.com/api/72f988bf-86f1-41af-91ab-2d7cd011db47</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"5\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Owner\"><vt:lpwstr>slreznit@microsoft.com</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"6\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_SetDate\"><vt:lpwstr>2017-09-05T13:44:33.6592363+03:00</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"7\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Name\"><vt:lpwstr>General</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"8\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Application\"><vt:lpwstr>Microsoft Azure Information Protection</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"9\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Extended_MSFT_Method\"><vt:lpwstr>Manual</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"10\" name=\"Sensitivity\"><vt:lpwstr>General</vt:lpwstr></property></Properties>";
  EXPECT_EQ(xml, expectedXml) << "Wrong content of the entry";
}

TEST(ZipTests, MissingEntry_ThrowsZipException) {
  auto stream = GetIStreamFromFile("LabeledGeneral.docx");
  ZipFile zipFile(stream);
  EXPECT_THROW(zipFile.GetEntry("docProps/missing.xml"), ZipEntryNotFoundException) << "The entry should not be in the zip file";
}

TEST(ZipTests, NotAZip_ThrowsZipException) {
  auto stream = GetIStreamFromFile("test.txt");
  EXPECT_THROW(ZipFile f(stream), ZipException) << "Should not create a ZipFile object with non-zip file";
}

TEST(ZipTests, SetEntry_EntrySet) {
  auto stream = GetIStreamFromFile("LabeledGeneral.docx");
  ZipFile zipFile(stream);
  zipFile.SetEntry("docProps/custom.xml", "Ameer assi");
  zipFile.SetEntry("docProps/custom1.xml", "Ameer assi1");
  zipFile.SetEntry("docProps/custom2.xml", "Ameer assi2");
  zipFile.SetEntry("docProps/custom3.xml", "Ameer assi3");

  zipFile.Commit(nullptr);
  string entry = zipFile.GetEntry("docProps/custom.xml");
  EXPECT_EQ(entry, "Ameer assi");
}

} // namespace mip
} // namespace file
