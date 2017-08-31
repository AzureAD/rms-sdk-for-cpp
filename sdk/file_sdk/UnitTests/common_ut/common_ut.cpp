#include <QString>
#include <QtTest>
#include <file_format_factory.h>
#include <XMPFileFormat/xmp_file_format.h>
#include <CompoundFileFormat/compound_file_format.h>
#include <OPCFileFormat/opc_file_format.h>
#include <PDFFileFormat/pdf_file_format.h>
#include <PFileFileFormat/pfile_file_format.h>
#include <DefaultFormat/default_format.h>
#include "istream_mock.h"

Q_DECLARE_METATYPE(std::string)

using std::string;
using mip::file::FileFormatFactory;
using mip::file::XMPFileFormat;
using mip::file::CompoundFileFormat;
using mip::file::OPCFileFormat;
using mip::file::PDFFileFormat;
using mip::file::PFileFileFormat;
using mip::file::DefaultFormat;

class CommonTests : public QObject
{
  Q_OBJECT

public:
  CommonTests();

private Q_SLOTS:
  void Create_StreamIsNull_ThrowInvalidArgument();
  void Create_NoExtension_ThrowInvalidArgument();
  void Create_ExtensionWithoutDot_AddsDot();
  void Create_ExtensionWithoutDot_AddsDot_data(){
    QTest::addColumn<std::string>("extension");
    QTest::addColumn<std::string>("result");

    QTest::newRow("docx") << std::string("docx") << std::string(".docx");
    QTest::newRow("xxx.pfile") << std::string("xxx.pfile") << std::string(".xxx");
  }

  void Create_XMPExtension_ReturnXMPFormat();
  void Create_XMPExtension_ReturnXMPFormat_data() {
    QTest::addColumn<std::string>("extension");

    QTest::newRow(".jpg") << std::string(".jpg");
    QTest::newRow(".jpeg") << std::string(".jpeg");
    QTest::newRow(".jpe") << std::string(".jpe");
    QTest::newRow(".jif") << std::string(".jif");
    QTest::newRow(".jfif") << std::string(".jfif");
    QTest::newRow(".jfi") << std::string(".jfi");
    QTest::newRow(".png") << std::string(".png");
    QTest::newRow(".tif") << std::string(".tif");
    QTest::newRow(".tiff") << std::string(".tiff");
    QTest::newRow(".gif") << std::string(".gif");
    QTest::newRow(".dng") << std::string(".dng");
    QTest::newRow(".psd") << std::string(".psd");
  }

  void Create_CompoundFileExtension_ReturnCompoundFileFormat();
  void Create_CompoundFileExtension_ReturnCompoundFileFormat_data() {
    QTest::addColumn<std::string>("extension");

    QTest::newRow(".doc") << std::string(".doc");
    QTest::newRow(".xls") << std::string(".xls");
    QTest::newRow(".xlt") << std::string(".xlt");
    QTest::newRow(".ppt") << std::string(".ppt");
    QTest::newRow(".pps") << std::string(".pps");
    QTest::newRow(".pot") << std::string(".pot");
    QTest::newRow(".vsd") << std::string(".vsd");
    QTest::newRow(".vdw") << std::string(".vdw");
    QTest::newRow(".vst") << std::string(".vst");
    QTest::newRow(".pub") << std::string(".pub");
    QTest::newRow(".sldprt") << std::string(".sldprt");
    QTest::newRow(".slddrw") << std::string(".slddrw");
    QTest::newRow(".sldasm") << std::string(".sldasm");
    QTest::newRow(".mpp") << std::string(".mpp");
    QTest::newRow(".mpt") << std::string(".mpt");
  }

  void Create_OPCFileExtension_ReturnOPCFileFormat();
  void Create_OPCFileExtension_ReturnOPCFileFormat_data() {
    QTest::addColumn<std::string>("extension");

    QTest::newRow(".docx") << std::string(".docx");
    QTest::newRow(".docm") << std::string(".docm");
    QTest::newRow(".dotm") << std::string(".dotm");
    QTest::newRow(".dotx") << std::string(".dotx");
    QTest::newRow(".xlsx") << std::string(".xlsx");
    QTest::newRow(".xltx") << std::string(".xltx");
    QTest::newRow(".xltm") << std::string(".xltm");
    QTest::newRow(".xlsm") << std::string(".xlsm");
    QTest::newRow(".xlsb") << std::string(".xlsb");
    QTest::newRow(".pptx") << std::string(".pptx");
    QTest::newRow(".ppsx") << std::string(".ppsx");
    QTest::newRow(".pptm") << std::string(".pptm");
    QTest::newRow(".ppsm") << std::string(".ppsm");
    QTest::newRow(".potx") << std::string(".potx");
    QTest::newRow(".potm") << std::string(".potm");
    QTest::newRow(".vsdx") << std::string(".vsdx");
    QTest::newRow(".vsdm") << std::string(".vsdm");
    QTest::newRow(".vssx") << std::string(".vssx");
    QTest::newRow(".vssm") << std::string(".vssm");
    QTest::newRow(".vstx") << std::string(".vstx");
    QTest::newRow(".vstm") << std::string(".vstm");
    QTest::newRow(".xps") << std::string(".xps");
    QTest::newRow(".oxps") << std::string(".oxps");
    QTest::newRow(".dwfx") << std::string(".dwfx");
  }

  void Create_PDFFileExtension_ReturnPDFFileFormat();
  void Create_PDFFileExtension_ReturnPDFFileFormat_data() {
    QTest::addColumn<std::string>("extension");

    QTest::newRow(".pdf") << std::string(".pdf");
  }

  void Create_PFileFileExtension_ReturnPFileFileFormat();
  void Create_PFileFileExtension_ReturnPFileFileFormat_data() {
    QTest::addColumn<std::string>("extension");

    QTest::newRow(".pfile") << std::string(".pfile");
    QTest::newRow(".xxx.pfile") << std::string(".xxx.pfile");
    QTest::newRow(".ppdf") << std::string(".ppdf");
    QTest::newRow(".ptxt") << std::string(".ptxt");
    QTest::newRow(".pxml") << std::string(".pxml");
    QTest::newRow(".pjpg") << std::string(".pjpg");
    QTest::newRow(".pjpeg") << std::string(".pjpeg");
    QTest::newRow(".ppng") << std::string(".ppng");
    QTest::newRow(".ptif") << std::string(".ptif");
    QTest::newRow(".ptiff") << std::string(".ptiff");
    QTest::newRow(".pbmp") << std::string(".pbmp");
    QTest::newRow(".pgif") << std::string(".pgif");
    QTest::newRow(".pjpe") << std::string(".pjpe");
    QTest::newRow(".pjfif") << std::string(".pjfif");
    QTest::newRow(".pjt") << std::string(".pjt");
  }

  void Create_UknonwExtension_ReturnPFileFileFormat();
};

CommonTests::CommonTests()
{
}

void CommonTests::Create_StreamIsNull_ThrowInvalidArgument() {
  QVERIFY_EXCEPTION_THROWN(FileFormatFactory::Create(nullptr, ""), std::invalid_argument);
}

void CommonTests::Create_NoExtension_ThrowInvalidArgument() {
  auto stream = std::make_shared<IStreamMock>();
  QVERIFY_EXCEPTION_THROWN(FileFormatFactory::Create(stream, ""), std::invalid_argument);
}

void CommonTests::Create_ExtensionWithoutDot_AddsDot() {
  QFETCH(std::string, extension);
  QFETCH(std::string, result);

  if (extension == std::string("xxx.pfile")) {
    QSKIP("get original extention of pfile is not impl yet");
  }

  auto stream = std::make_shared<IStreamMock>();
  auto fileFormat = FileFormatFactory::Create(stream, extension);
  QVERIFY2(fileFormat->GetOriginalExtension() == result, "didn't add . to the extension");
}

void CommonTests::Create_XMPExtension_ReturnXMPFormat() {
  QFETCH(std::string, extension);
  auto stream = std::make_shared<IStreamMock>();
  auto fileFormat = FileFormatFactory::Create(stream, extension);
  QVERIFY2(dynamic_cast<XMPFileFormat*>(fileFormat.get()) != NULL, "is not XMPFileFormat");
}

void CommonTests::Create_CompoundFileExtension_ReturnCompoundFileFormat() {
  QFETCH(std::string, extension);
  auto stream = std::make_shared<IStreamMock>();
  auto fileFormat = FileFormatFactory::Create(stream, extension);
  QVERIFY2(dynamic_cast<CompoundFileFormat*>(fileFormat.get()) != NULL, "is not CompoundFileFormat");
}

void CommonTests::Create_OPCFileExtension_ReturnOPCFileFormat() {
  QFETCH(std::string, extension);
  auto stream = std::make_shared<IStreamMock>();
  auto fileFormat = FileFormatFactory::Create(stream, extension);
  QVERIFY2(dynamic_cast<OPCFileFormat*>(fileFormat.get()) != NULL, "is not OPCFileFormat");
}

void CommonTests::Create_PDFFileExtension_ReturnPDFFileFormat() {
  QFETCH(std::string, extension);
  auto stream = std::make_shared<IStreamMock>();
  auto fileFormat = FileFormatFactory::Create(stream, extension);
  QVERIFY2(dynamic_cast<PDFFileFormat*>(fileFormat.get()) != NULL, "is not PDFFileFormat");
}

void CommonTests::Create_PFileFileExtension_ReturnPFileFileFormat() {
  QFETCH(std::string, extension);
  auto stream = std::make_shared<IStreamMock>();
  auto fileFormat = FileFormatFactory::Create(stream, extension);
  QVERIFY2(dynamic_cast<PFileFileFormat*>(fileFormat.get()) != NULL, "is not PFileFileFormat");
}

void CommonTests::Create_UknonwExtension_ReturnPFileFileFormat() {
  std::string extension (".blabla");
  auto stream = std::make_shared<IStreamMock>();
  auto fileFormat = FileFormatFactory::Create(stream, extension);
  QVERIFY2(dynamic_cast<DefaultFormat*>(fileFormat.get()) != NULL, "is not DefaultFormat");
}

QTEST_APPLESS_MAIN(CommonTests)

#include "common_ut.moc"
