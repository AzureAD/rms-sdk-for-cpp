#include <QString>
#include <QtTest>
#include <Qt>
#include "../../OPCFileFormat/zipapi.h"
#include <iostream>

class ZipTests : public QObject
{
  Q_OBJECT

public:
  ZipTests();

private Q_SLOTS:
//  void testCase();
  void testCaseGetEntryReturnsXml();
  void testCaseMissingEntryThrowZipException();
  void testCaseNotAZipThrowZipException();
};

ZipTests::ZipTests()
{
}

//void ZipTests::testCase()
//{
//  try
//  {
//  std::cout << "start" << std::endl;
//  auto zip = ZipApi();

//  std::cout << "get entry first time" << std::endl;
//  auto val1 = zip.GetEntry("\\\\sislands\\Public\\RnD\\rms-sdk\\Tests\\test.txt", "docProps/custom.xml");
//  QVERIFY2(val1 != "", "entry1 is not exists");
//  std::cout << val1 << std::endl;
//  }
//  catch(const std::exception &ex)
//  {
//    std::cout << ex.what() << std::endl;
//  }
//}

void ZipTests::testCaseGetEntryReturnsXml()
{
  auto xml = ZipApi().GetEntry("\\\\sislands\\Public\\RnD\\rms-sdk\\Tests\\LabeledGeneral.docx", "docProps/custom.xml");
  QVERIFY2(xml != "", "XML is empty");
  std::cout << xml << std::endl;
  //QCOMPARE(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/custom-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\"><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"2\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Enabled\"><vt:lpwstr>True</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"3\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_SiteId\"><vt:lpwstr>72f988bf-86f1-41af-91ab-2d7cd011db47</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"4\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Ref\"><vt:lpwstr>https://api.informationprotection.azure.com/api/72f988bf-86f1-41af-91ab-2d7cd011db47</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"5\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Owner\"><vt:lpwstr>slreznit@microsoft.com</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"6\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_SetDate\"><vt:lpwstr>2017-09-05T13:44:33.6592363+03:00</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"7\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Name\"><vt:lpwstr>General</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"8\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Application\"><vt:lpwstr>Microsoft Azure Information Protection</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"9\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Extended_MSFT_Method\"><vt:lpwstr>Manual</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"10\" name=\"Sensitivity\"><vt:lpwstr>General</vt:lpwstr></property></Properties>");
}

void ZipTests::testCaseMissingEntryThrowZipException()
{
  QVERIFY_EXCEPTION_THROWN(ZipApi().GetEntry("\\\\sislands\\Public\\RnD\\rms-sdk\\Tests\\LabeledGeneral.docx", "docProps/missing.xml"), ZipEntryNotFoundException);
}

void ZipTests::testCaseNotAZipThrowZipException()
{
  QVERIFY_EXCEPTION_THROWN(ZipApi().GetEntry("\\\\sislands\\Public\\RnD\\rms-sdk\\Tests\\test.txt", "docProps/custom.xml"), ZipException);
}

QTEST_APPLESS_MAIN(ZipTests)

#include "zip_tests.moc"
