#include "common_ut.h"
#include <Common/file_format_factory.h>
#include <XMPFileFormat/xmp_file_format.h>
#include <CompoundFileFormat/compound_file_format.h>
#include <OPCFileFormat/opc_file_format.h>
#include <PDFFileFormat/pdf_file_format.h>
#include <PFileFileFormat/pfile_file_format.h>
#include <DefaultFormat/default_format.h>
#include "istream_mock.h"

using std::string;
using mip::file::FileFormatFactory;
using mip::file::XMPFileFormat;
using mip::file::CompoundFileFormat;
using mip::file::OPCFileFormat;
using mip::file::PDFFileFormat;
using mip::file::PFileFileFormat;
using mip::file::DefaultFormat;

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
