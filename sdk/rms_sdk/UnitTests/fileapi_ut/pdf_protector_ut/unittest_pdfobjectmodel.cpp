#include "unittest_pdfobjectmodel.h"
#include "Auth.h"
#include "depend.h"
#include "PDFObjectModel/pdf_object_model.h"
#include "pdfobjectmodel_mock.h"
#include "PFileProtector.h"
#include <stdio.h>
#include<cstring>
#include <string>
#include<windows.h>
#include "basic.h"

namespace pdfobjectmodel = rmscore::pdfobjectmodel;
namespace objmodel_unittest = rmscore_unittest::fileapi_unittest;

void PDFCreator_CreateCustomEncryptedFile::SetUpTestCase() {
}

void PDFCreator_CreateCustomEncryptedFile::TearDownTestCase() {
}

TEST_P(PDFCreator_CreateCustomEncryptedFile, CreateCustomEncryptedFile_T) {
  CreateCustomEncryptedFile_P TParam = GetParam();
  pdfobjectmodel::PDFModuleMgr::Initialize();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;
  std::string fileOut = unittests::dependency::GetCurrentInputFile() + TParam.fileout;
  auto inFile = std::make_shared<std::fstream>(
      fileIn,
      std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  auto OutFile = std::make_shared<std::fstream>(
      fileOut,
      std::ios_base::in | std::ios_base::out | std::ios_base::binary|std::ios_base::trunc);
  std::shared_ptr<std::iostream> inFile_stream = inFile;
  pdfobjectmodel::PDFSharedStream inputFileData =
      std::make_shared<objmodel_unittest::PDFSharedStream_mock>(inFile_stream);
  std::shared_ptr<std::iostream> OutFile_stream = OutFile;
  pdfobjectmodel::PDFSharedStream output_FileData =
      std::make_shared<objmodel_unittest::PDFSharedStream_mock>(OutFile_stream);
  std::unique_ptr<pdfobjectmodel::PDFCreator> pdf_creator_ = pdfobjectmodel::PDFCreator::Create();
  std::string filter_name = TParam.filter_name;
  std::vector<unsigned char> publishing_license;
  std::string publishing_license_string ="<?xml version=\"1.0\"?><XrML xmlns=\"\" version=\"1.2\"><BODY type=\"Microsoft Rights Label\" version=\"3.0\">\0";
  for(unsigned int i = 0; i < publishing_license_string.size(); i++){
    publishing_license.push_back(publishing_license_string[i]);
    publishing_license.push_back(NULL);
  }
  std::string cache_file_path = fileIn;
  cache_file_path += PROGRESSIVE_ENCRYPT_TEMP_FILE;
  auto mock_PDFCryptoHandler = std::make_shared<objmodel_unittest::PDFCryptoHandler_mock>();
  rmscore::pdfobjectmodel::PDFCreatorErr ret;
  try {
    ret = pdf_creator_->CreateCustomEncryptedFile(
        inputFileData,
        cache_file_path,
        filter_name,
        publishing_license,
        mock_PDFCryptoHandler,
        output_FileData);
  } catch (const rmsauth::Exception& e) {
     std::string message(e.what());
     EXPECT_EQ(TParam.ExceptionsMess, message);;
     return;
  } catch (const rmscore::exceptions::RMSPDFFileException& e) {
     std::string message(e.what());
     EXPECT_EQ(TParam.ExceptionsMess, message);;
     return;
  } catch (const rmscore::exceptions::RMSException& e) {
     std::string message(e.what());
     EXPECT_EQ(TParam.ExceptionsMess, message);
     return;
  }
  EXPECT_EQ(TParam.ret, ret);
}
INSTANTIATE_TEST_CASE_P(, PDFCreator_CreateCustomEncryptedFile, testing::Values(
    CreateCustomEncryptedFile_P("Input/package.pdf","OutPut/CreateCustomEncryptedFile/package.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::SUCCESS),
    CreateCustomEncryptedFile_P("Input/demage.pdf","OutPut/CreateCustomEncryptedFile/demage.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::FORMAT),
    CreateCustomEncryptedFile_P("Input/XFAStatic.pdf","OutPut/CreateCustomEncryptedFile/XFAStatic.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::SUCCESS),
    CreateCustomEncryptedFile_P("Input/sign.pdf","OutPut/CreateCustomEncryptedFile/sign.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::FORMAT),
    CreateCustomEncryptedFile_P("Input/XFADyanmic.pdf","OutPut/CreateCustomEncryptedFile/XFADyanmic.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::FORMAT),
    CreateCustomEncryptedFile_P("Input/Protector/cer2-no.pdf","OutPut/CreateCustomEncryptedFile/cer2-no.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SECURITY),
    CreateCustomEncryptedFile_P("Input/Protector/cer.pdf","OutPut/CreateCustomEncryptedFile/cer.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SECURITY),
    CreateCustomEncryptedFile_P("Input/Protector/password.pdf","OutPut/CreateCustomEncryptedFile/password.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SECURITY),
    CreateCustomEncryptedFile_P("Input/Protector/pwd_protect.pdf","OutPut/CreateCustomEncryptedFile/pwd_protect.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SECURITY),
    CreateCustomEncryptedFile_P("Input/Test.txt","OutPut/CreateCustomEncryptedFile/txt.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT)

));

void PDFCreator_UnprotectCustomEncryptedFile::SetUpTestCase() {

}

void PDFCreator_UnprotectCustomEncryptedFile::TearDownTestCase() {

}

TEST_P(PDFCreator_UnprotectCustomEncryptedFile, UnprotectCustomEncryptedFile_T) {
  UnprotectCustomEncryptedFile_P TParam = GetParam();
  pdfobjectmodel::PDFModuleMgr::Initialize();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;
  std::string fileOut = unittests::dependency::GetCurrentInputFile() + TParam.fileout;
  auto inFile = std::make_shared<std::fstream>(
      fileIn,
      std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  auto OutFile = std::make_shared<std::fstream>(
      fileOut,
      std::ios_base::in | std::ios_base::out | std::ios_base::binary|std::ios_base::trunc);
  std::shared_ptr<std::iostream> inFile_stream=inFile;
  pdfobjectmodel::PDFSharedStream inputFileData =
  std::make_shared<objmodel_unittest::PDFSharedStream_mock>(inFile_stream);
  std::shared_ptr<std::iostream> OutFile_stream = OutFile;
  pdfobjectmodel::PDFSharedStream output_FileData =
  std::make_shared<objmodel_unittest::PDFSharedStream_mock>(OutFile_stream);
  std::unique_ptr<pdfobjectmodel::PDFCreator> pdf_creator_ = pdfobjectmodel::PDFCreator::Create();
  std::string filter_name = TParam.filter_name;
  rmscore::pdfobjectmodel::PDFCreatorErr ret;
  auto mock_SecurityHandler = std::make_shared<objmodel_unittest::PDFSecurityHandler_mock>();
  std::string cache_file_path = fileIn;
  cache_file_path += CREATOR_STREAM_TEMP_FILE;
  ret = pdf_creator_->UnprotectCustomEncryptedFile(
      inputFileData,
      cache_file_path,
      filter_name,
      mock_SecurityHandler,
      output_FileData);
  EXPECT_EQ(TParam.ret, ret);
}
INSTANTIATE_TEST_CASE_P(, PDFCreator_UnprotectCustomEncryptedFile, testing::Values(
    UnprotectCustomEncryptedFile_P("OutPut/CreateCustomEncryptedFile/XFAStatic.pdf","OutPut/UnprotectCustomEncryptedFile/XFAStatic.pdf",PDF_PROTECTOR_FILTER_NAME,pdfobjectmodel::PDFCreatorErr::SUCCESS),
    UnprotectCustomEncryptedFile_P("OutPut/CreateCustomEncryptedFile/package.pdf","OutPut/UnprotectCustomEncryptedFile/package.pdf",PDF_PROTECTOR_FILTER_NAME,pdfobjectmodel::PDFCreatorErr::SUCCESS),
    UnprotectCustomEncryptedFile_P("Input/demage.pdf","OutPut/UnprotectCustomEncryptedFile/demage.pdf",PDF_PROTECTOR_FILTER_NAME,pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/Protector/cer.pdf","OutPut/UnprotectCustomEncryptedFile/cer.pdf",PDF_PROTECTOR_FILTER_NAME,pdfobjectmodel::PDFCreatorErr::SECURITY),
    UnprotectCustomEncryptedFile_P("Input/Test.txt","OutPut/UnprotectCustomEncryptedFile/txt.pdf",PDF_PROTECTOR_FILTER_NAME,pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/Protector/password.pdf","OutPut/UnprotectCustomEncryptedFile/password.pdf",PDF_PROTECTOR_FILTER_NAME,pdfobjectmodel::PDFCreatorErr::SECURITY)
 ));
