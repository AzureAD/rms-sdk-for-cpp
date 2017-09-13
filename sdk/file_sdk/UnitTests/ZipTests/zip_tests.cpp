#include <QString>
#include <QtTest>
#include <Qt>
#include "../../OPCFileFormat/zipapi.h"
#include <iostream>
#include <QTStreamImpl.h>
#include <memory>
#include <CryptoAPI.h>
#include <sstream>
#include <fstream>
#include <stdio.h>

class ZipTests : public QObject
{
    Q_OBJECT

public:
    ZipTests();

private Q_SLOTS:
    void testCaseGetEntryReturnsXml();
    void testCaseMissingEntryThrowZipException();
    void testCaseNotAZipThrowZipException();
    void testSetEntrySetsXml();
};

using rmscrypto::api::IStream;
using std::static_pointer_cast;
using std::make_shared;
using std::shared_ptr;
using std::string;

const string notLabeledInputFilePath = "\\\\sislands\\Public\\RnD\\rms-sdk\\Tests\\NotLabeled.docx";
const string labeledInputFilePath = "\\\\sislands\\Public\\RnD\\rms-sdk\\Tests\\LabeledGeneral.docx";
const string outputFilePath = "\\\\sislands\\Public\\RnD\\rms-sdk\\Tests\\Output.docx";

ZipTests::ZipTests()
{
}

shared_ptr<IStream> GetIStreamFromFile(const std::string& filePath)
{
    auto ifs = make_shared<std::ifstream>(filePath, std::ios_base::binary);
    return rmscrypto::api::CreateStreamFromStdStream(std::static_pointer_cast<std::istream>(ifs));
}

void ZipTests::testSetEntrySetsXml()
{
   ZipApi a;
   std::map<std::string, std::string> map;
   map["docProps/custom.xml"] = "Labels for Ameer";
   map["word/document.xml"] = "bla bla bla";

   a.SetEntry(notLabeledInputFilePath, outputFilePath, map);

   QCOMPARE(QString("Labels for Ameer"), QString(a.GetEntry(GetIStreamFromFile(outputFilePath), "docProps/custom.xml").c_str()));
   QCOMPARE(QString("bla bla bla"), QString(a.GetEntry(GetIStreamFromFile(outputFilePath), "word/document.xml").c_str()));
   remove(outputFilePath.c_str()); // removing the file which we created.
}

void ZipTests::testCaseGetEntryReturnsXml()
{
    auto stream = GetIStreamFromFile(labeledInputFilePath);
    auto xml = ZipApi().GetEntry(stream, "docProps/custom.xml");
    QVERIFY2(xml != "", "XML is empty");
    QCOMPARE(QString(xml.c_str()), QString("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/custom-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\"><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"2\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Enabled\"><vt:lpwstr>True</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"3\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_SiteId\"><vt:lpwstr>72f988bf-86f1-41af-91ab-2d7cd011db47</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"4\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Ref\"><vt:lpwstr>https://api.informationprotection.azure.com/api/72f988bf-86f1-41af-91ab-2d7cd011db47</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"5\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Owner\"><vt:lpwstr>slreznit@microsoft.com</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"6\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_SetDate\"><vt:lpwstr>2017-09-05T13:44:33.6592363+03:00</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"7\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Name\"><vt:lpwstr>General</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"8\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Application\"><vt:lpwstr>Microsoft Azure Information Protection</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"9\" name=\"MSIP_Label_f42aa342-8706-4288-bd11-ebb85995028c_Extended_MSFT_Method\"><vt:lpwstr>Manual</vt:lpwstr></property><property fmtid=\"{D5CDD505-2E9C-101B-9397-08002B2CF9AE}\" pid=\"10\" name=\"Sensitivity\"><vt:lpwstr>General</vt:lpwstr></property></Properties>"));
}

void ZipTests::testCaseMissingEntryThrowZipException()
{
    auto stream = GetIStreamFromFile(labeledInputFilePath);
    QVERIFY_EXCEPTION_THROWN(ZipApi().GetEntry(stream, "docProps/missing.xml"), ZipEntryNotFoundException);
}

void ZipTests::testCaseNotAZipThrowZipException()
{
    auto stream = GetIStreamFromFile("\\\\sislands\\Public\\RnD\\rms-sdk\\Tests\\test.txt");
    QVERIFY_EXCEPTION_THROWN(ZipApi().GetEntry(stream, "docProps/custom.xml"), ZipException);
}

QTEST_APPLESS_MAIN(ZipTests)

#include "zip_tests.moc"
