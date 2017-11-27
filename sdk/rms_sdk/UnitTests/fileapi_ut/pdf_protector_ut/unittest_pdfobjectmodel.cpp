#include "unittest_pdfobjectmodel.h"
#include "Auth.h"
#include "depend.h"
#include "PDFObjectModel/pdf_object_model.h"
#include "UserPolicy.h"
#include "PDFProtector_child.h"
#include "PFileProtector.h"
#include "Core/ProtectionPolicy.h"

namespace pdfobjectmodel = rmscore::pdfobjectmodel;
namespace fileapi = rmscore::fileapi;
namespace modernapi = rmscore::modernapi;

static std::shared_ptr<modernapi::UserPolicy> user_policy_;
static void SetUserPolicy() {
  pdfobjectmodel::PDFModuleMgr::Initialize();//init
  //**********get template info**************
  AuthCallback auth(CLIENTID, REDIRECTURL);
  ConsentCallback consent;
  modernapi::AppDataHashMap signedData;
  std::shared_ptr<std::atomic<bool> > cancelState(new std::atomic<bool>(false));
  auto templates_future = modernapi::TemplateDescriptor::GetTemplateListAsync(
      CLIENTEMAIL,
      auth,
      std::launch::deferred,
      cancelState);
  auto templates = templates_future.get();
  fileapi::ProtectWithTemplateOptions pt(fileapi::CryptoOptions::AES128_ECB,
                                         (*templates)[0], signedData, true);//设置模板
  //**********************************
  fileapi::UserContext ut (CLIENTEMAIL, auth, consent);

  auto userPolicyCreationOptions = modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction;
  userPolicyCreationOptions = static_cast<rmscore::modernapi::UserPolicyCreationOptions>(
      userPolicyCreationOptions |
      rmscore::modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);

  user_policy_ = modernapi::UserPolicy::CreateFromTemplateDescriptor(
      pt.templateDescriptor,
      ut.userId,
      ut.authenticationCallback,
      userPolicyCreationOptions,
      pt.signedAppData,
      cancelState);
}

void PDFCreator_CreateCustomEncryptedFile::SetUpTestCase() {
  SetUserPolicy();
}

void PDFCreator_CreateCustomEncryptedFile::TearDownTestCase() {

}

TEST_P(PDFCreator_CreateCustomEncryptedFile, CreateCustomEncryptedFile_T) {
  CreateCustomEncryptedFile_P TParam = GetParam();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;
  //std::string fileIns=unittests::dependency::GetCurrentInputFile() "Input/Protector/anyoness.pdf";

  auto inFile = std::make_shared<std::fstream>(
      fileIn,
      std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  std::unique_ptr<pdfobjectmodel::PDFCreator> pdf_creator_ = pdfobjectmodel::PDFCreator::Create();

  auto encryptedSS = std::make_shared<std::stringstream>();
  std::shared_ptr<std::iostream> encryptedIOS = encryptedSS;
  auto output_encrypted = rmscrypto::api::CreateStreamFromStdStream(encryptedIOS);

  std::string originalFileExtension = ".pFile";

  auto p_PDFprotector = std::make_shared<fileapi::PDFProtector_unit>(fileIn, originalFileExtension, inFile);
  auto crypto_hander = std::make_shared<fileapi::PDFCryptoHandler_child>(p_PDFprotector);

  p_PDFprotector->SetUserPolicy(user_policy_);

  std::string filter_name = TParam.filter_name;
  std::vector<unsigned char> publishing_license = user_policy_->SerializedPolicy();

  uint32_t ret;
  try {
    ret = pdf_creator_->CreateCustomEncryptedFile(
        fileIn,
        filter_name,
        publishing_license,
        crypto_hander,
        output_encrypted);
  }
  catch (const rmsauth::Exception& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);;
    return;
  }
  catch (const rmscore::exceptions::RMSPDFFileException& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);;
    return;
  }
  catch (const rmscore::exceptions::RMSException& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }
  EXPECT_EQ(TParam.ret, ret);
  if (ret==pdfobjectmodel::PDFCreatorErr::SUCCESS) {
    //完全加密
    std::string wrapper_in = unittests::dependency::GetCurrentInputFile() + "Input/wrapper.pdf";

    auto inwrapper = std::make_shared<std::fstream>(
        wrapper_in, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    std::shared_ptr<std::iostream> input_wrapper_IO = inwrapper;
    auto input_wrapper = rmscrypto::api::CreateStreamFromStdStream(input_wrapper_IO);
    std::unique_ptr<pdfobjectmodel::PDFUnencryptedWrapperCreator> pdf_wrapper_creator_ = pdfobjectmodel::PDFUnencryptedWrapperCreator::Create(input_wrapper);
    pdf_wrapper_creator_ = pdfobjectmodel::PDFUnencryptedWrapperCreator::Create(input_wrapper);
    pdf_wrapper_creator_->SetPayloadInfo(
        PDF_PROTECTOR_WRAPPER_SUBTYPE,
        PDF_PROTECTOR_WRAPPER_FILENAME,
        PDF_PROTECTOR_WRAPPER_DES,
        PDF_PROTECTOR_WRAPPER_VERSION);
    pdf_wrapper_creator_->SetPayLoad(output_encrypted);

    std::string file_out = unittests::dependency::GetCurrentInputFile()+ TParam.fileout;
    auto output_stream = std::make_shared<std::fstream>(
        file_out,
        std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    std::shared_ptr<std::iostream> outputIO = output_stream;
    auto output_wrapper = rmscrypto::api::CreateStreamFromStdStream(outputIO);
    bool result = pdf_wrapper_creator_->CreateUnencryptedWrapper(output_wrapper);
    EXPECT_EQ(1, result);
  }
}
INSTANTIATE_TEST_CASE_P(, PDFCreator_CreateCustomEncryptedFile, testing::Values(
    CreateCustomEncryptedFile_P("Input/unprotector.pdf","OutPut/CreateCustomEncryptedFile/FoxitIRMServices.pdf","FoxitIRMServices","NO Exception",pdfobjectmodel::PDFCreatorErr::SUCCESS),//不挂就好
    CreateCustomEncryptedFile_P("Input/package.pdf","OutPut/CreateCustomEncryptedFile/package.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::SUCCESS),
    CreateCustomEncryptedFile_P("Input/demage.pdf","OutPut/CreateCustomEncryptedFile/demage.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::FORMAT),
    CreateCustomEncryptedFile_P("Input/XFAStatic.pdf","OutPut/CreateCustomEncryptedFile/XFAStatic.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::SUCCESS),
    CreateCustomEncryptedFile_P("Input/sign.pdf","OutPut/CreateCustomEncryptedFile/sign.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::FORMAT),//未加密的文档
    CreateCustomEncryptedFile_P("Input/XFADyanmic-crash.pdf","OutPut/CreateCustomEncryptedFile/XFADyanmic-crash.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::FORMAT),//未加密的文档
    CreateCustomEncryptedFile_P("Input/XFADyanmic.pdf","OutPut/CreateCustomEncryptedFile/XFADyanmic.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::FORMAT),//未加密的文档
    //CreateCustomEncryptedFile_P("Input/Protector/cer2-no.pdf","OutPut/CreateCustomEncryptedFile/cer2-no.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SECURITY),
    CreateCustomEncryptedFile_P("Input/Protector/cer.pdf","OutPut/CreateCustomEncryptedFile/cer.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SECURITY),
    CreateCustomEncryptedFile_P("Input/error/anyone.pdf","OutPut/CreateCustomEncryptedFile/error.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FILE),
    CreateCustomEncryptedFile_P("Input/Protector/anyone.pdf","OutPut/CreateCustomEncryptedFile/anyone.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::SUCCESS),
    CreateCustomEncryptedFile_P("Input/Protector/password.pdf","OutPut/CreateCustomEncryptedFile/password.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SECURITY),
    CreateCustomEncryptedFile_P("Input/Protector/pwd_protect.pdf","OutPut/CreateCustomEncryptedFile/pwd_protect.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SECURITY),
    CreateCustomEncryptedFile_P("Input/Test.txt","OutPut/CreateCustomEncryptedFile/txt.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),//未加密的文档
    CreateCustomEncryptedFile_P("Input/unprotector.pdf","OutPut/CreateCustomEncryptedFile/protector.pdf",PDF_PROTECTOR_FILTER_NAME,"NO Exception",pdfobjectmodel::PDFCreatorErr::SUCCESS)//未加密的文档
));

void PDFCreator_UnprotectCustomEncryptedFile::SetUpTestCase() {

}

void PDFCreator_UnprotectCustomEncryptedFile::TearDownTestCase() {

}

TEST_P(PDFCreator_UnprotectCustomEncryptedFile, UnprotectCustomEncryptedFile_T) {
  UnprotectCustomEncryptedFile_P TParam = GetParam();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;
  auto inFile = std::make_shared<std::fstream>(
      fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::string originalFileExtension = ".pFile";

  auto p_PDFprotector = std::make_shared<fileapi::PDFProtector_unit>(fileIn, originalFileExtension, inFile);

  //******************
  pdfobjectmodel::PDFModuleMgr::Initialize();

  //************************
  AuthCallback auth(CLIENTID, REDIRECTURL);
  ConsentCallback consent;
  rmscore::fileapi::UnprotectOptions upt (false, true);
  rmscore::fileapi::UserContext ut (CLIENTEMAIL, auth, consent);
  std::shared_ptr<std::atomic<bool> > cancelState(new std::atomic<bool>(false));
  //*****************************************
  auto security_hander = std::make_shared<fileapi::PDFSecurityHandler_child>(p_PDFprotector,ut,upt,cancelState);
  //**************************************************************
  std::unique_ptr<pdfobjectmodel::PDFCreator> pdf_creator_ = pdfobjectmodel::PDFCreator::Create();
  //****************
  std::string file_out = unittests::dependency::GetCurrentInputFile() + TParam.fileout;
  // create streams
  auto output_stream = std::make_shared<std::fstream>(
      file_out, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
  std::shared_ptr<std::iostream> output_decrypted_IO = output_stream;
  auto output_decrypted = rmscrypto::api::CreateStreamFromStdStream(output_decrypted_IO);
  std::string filter_name = TParam.filter_name;


  std::shared_ptr<std::iostream> input_encrypted_IO = inFile;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);
  std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =  pdfobjectmodel::PDFWrapperDoc::Create(input_encrypted);
  auto payloadSS = std::make_shared<std::stringstream>();
  std::shared_ptr<std::iostream> payloadIOS = payloadSS;
  auto output_payload = rmscrypto::api::CreateStreamFromStdStream(payloadIOS);
  bool bGetPayload = pdf_wrapper_doc->StartGetPayload(output_payload);
  //******************

  uint32_t ret ;
  try {
    ret = pdf_creator_->UnprotectCustomEncryptedFile(
        output_payload,
        filter_name,
        security_hander,
        output_decrypted);

  }
  catch (const rmsauth::Exception& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }
  catch (const rmscore::exceptions::RMSPDFFileException& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }
  catch (const rmscore::exceptions::RMSException& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }
  EXPECT_EQ(TParam.ret, ret);
  //*************
}
INSTANTIATE_TEST_CASE_P(, PDFCreator_UnprotectCustomEncryptedFile, testing::Values(
    UnprotectCustomEncryptedFile_P("Input/Protector/Protected package.pdf","OutPut/UnprotectCustomEncryptedFile/Protected package.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SUCCESS),
    UnprotectCustomEncryptedFile_P("Input/Protector/customerTemplate.pdf","OutPut/UnprotectCustomEncryptedFile/customerTemplate.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SECURITY),
    UnprotectCustomEncryptedFile_P("Input/Protector/demage.pdf","OutPut/UnprotectCustomEncryptedFile/demage.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/sign.pdf","OutPut/UnprotectCustomEncryptedFile/sign.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/XFADyanmic-crash.pdf","OutPut/UnprotectCustomEncryptedFile/XFADyanmic-crash.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/XFAStatic.pdf","OutPut/UnprotectCustomEncryptedFile/XFAStatic.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/Protector/Protected XFAStatic.pdf","OutPut/UnprotectCustomEncryptedFile/Protected XFAStatic.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SUCCESS),
    UnprotectCustomEncryptedFile_P("Input/Protector/cer.pdf","OutPut/UnprotectCustomEncryptedFile/cer.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/Test.txt","OutPut/UnprotectCustomEncryptedFile/txt.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/unprotector.pdf","OutPut/UnprotectCustomEncryptedFile/unprotector.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/error/error.pdf","OutPut/UnprotectCustomEncryptedFile/error.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/Protector/password.pdf","OutPut/UnprotectCustomEncryptedFile/password.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),
    UnprotectCustomEncryptedFile_P("Input/Protector/OfficeTemplate.ppdf","OutPut/UnprotectCustomEncryptedFile/OfficeTemplate.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::FORMAT),//Protector.h中的加密ppdf走的是另外路线，
    UnprotectCustomEncryptedFile_P("Input/Protector/MaxOwner.pdf","OutPut/UnprotectCustomEncryptedFile/MaxOwner.pdf",PDF_PROTECTOR_FILTER_NAME,"",pdfobjectmodel::PDFCreatorErr::SUCCESS),
    UnprotectCustomEncryptedFile_P("Input/Protector/phantomOfficeT.pdf","OutPut/UnprotectCustomEncryptedFile/anyone.pdf",PDF_PROTECTOR_FILTER_NAME,"Only the owner has the right to unprotect the document.",pdfobjectmodel::PDFCreatorErr::UNKNOWN_ERR)));

TEST_P(PDFWrapperDoc_GetWrapperType,GetWrapperType_T) {
  GetWrapperType_P TParam = GetParam();
  pdfobjectmodel::PDFModuleMgr::Initialize();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;

  auto inFile = std::make_shared<std::fstream>(fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::shared_ptr<std::iostream> input_encrypted_IO = inFile;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);
  uint32_t ret;
  try {
    std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =  pdfobjectmodel::PDFWrapperDoc::Create(input_encrypted);

    ret = pdf_wrapper_doc->GetWrapperType();
  }
  catch(const rmsauth::Exception& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }
  EXPECT_EQ(TParam.ret, ret);
}

INSTANTIATE_TEST_CASE_P(,PDFWrapperDoc_GetWrapperType,testing::Values(
    GetWrapperType_P("Input/Protector/anyone.pdf","No Exception",2),
    GetWrapperType_P("Input/Protector/V1V1.pdf","No Exception",1),//V1
    GetWrapperType_P("Input/Protector/OfficeTemplate.ppdf","No Exception",0),
    GetWrapperType_P("Input/Protector/V1V0.pdf","No Exception",2),
    GetWrapperType_P("Input/Protector/V0V0.pdf","No Exception",2),
    GetWrapperType_P("Input/Test.txt","No Exception",0),
    GetWrapperType_P("Input/Protector/cer.pdf","No Exception",0),
    GetWrapperType_P("Input/Protector/password.pdf","No Exception",0),
    GetWrapperType_P("Input/Errorr/Error.pdf","No Exception",0),
    GetWrapperType_P("Input/unprotector.pdf","No Exception",0)));

TEST_P(PDFWrapperDoc_GetCryptographicFilter,GetCryptographicFilter_T) {
  GetCryptographicFilter_P TParam = GetParam();
  pdfobjectmodel::PDFModuleMgr::Initialize();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;

  auto inFile = std::make_shared<std::fstream>(fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::shared_ptr<std::iostream> input_encrypted_IO = inFile;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);
  bool ret;
  std::wstring graphic_filter;
  float version_num;
  try {
    std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =  pdfobjectmodel::PDFWrapperDoc::Create(input_encrypted);

    ret = pdf_wrapper_doc->GetCryptographicFilter(graphic_filter, version_num);
  }
  catch(const rmsauth::Exception& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }
  EXPECT_EQ(TParam.ret, ret);
  if (TParam.ret == true) {
    EXPECT_EQ(TParam.graphic_filter, graphic_filter);
    EXPECT_EQ(TParam.version_num, version_num);
  }
}
INSTANTIATE_TEST_CASE_P(,PDFWrapperDoc_GetCryptographicFilter,testing::Values(
    GetCryptographicFilter_P("Input/Protector/OfficeTemplate.ppdf",L"",0,true,"NO Exception"),//
    GetCryptographicFilter_P("Input/Protector/test.pdf",L"Test",7,true,"NO Exception"),//自己设置的值，Test,7
    GetCryptographicFilter_P("Input/Protector/password.pdf",L"",0,true,"NO Exception"),
    GetCryptographicFilter_P("Input/Protector/error.pdf",L"",0,true,"NO Exception"),
    GetCryptographicFilter_P("Input/Protector/customerTemplate.pdf",L"FoxitRMSV2",1,true,"NO Exception"),
    GetCryptographicFilter_P("Input/unprotector.pdf",L"",0,true,"NO Exception"),
    GetCryptographicFilter_P("Input/Protector/V1V1.pdf",L"MicrosoftIRMServices",1,true,"NO Exception"),
    GetCryptographicFilter_P("Input/Protector/anyone.pdf",L"MicrosoftIRMServices",2,true,"NO Exception")));

TEST_P(PDFWrapperDoc_GetPayLoadSize, GetPayLoadSize_T) {
  GetPayLoadSize_P TParam = GetParam();
  pdfobjectmodel::PDFModuleMgr::Initialize();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;

  auto inFile = std::make_shared<std::fstream>(fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::shared_ptr<std::iostream> input_encrypted_IO = inFile;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);
  uint32_t PayLoadSize;
  try {
    std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =  pdfobjectmodel::PDFWrapperDoc::Create(input_encrypted);

    PayLoadSize = pdf_wrapper_doc->GetPayLoadSize();
  }
  catch(const rmsauth::Exception& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionMess, message);
    return;
  }
  EXPECT_EQ(TParam.PayLoadSize,PayLoadSize);
}

INSTANTIATE_TEST_CASE_P(,PDFWrapperDoc_GetPayLoadSize,testing::Values(
    GetPayLoadSize_P("Input/ERROR/Error.pdf",0,""),
    GetPayLoadSize_P("Input/Protector/Test.txt",0,""),
    GetPayLoadSize_P("Input/Protector/cer.pdf",0,""),
    GetPayLoadSize_P("Input/Protector/password.pdf",0,""),
    GetPayLoadSize_P("Input/Protector/OfficeTemplate.ppdf",0,""),
    GetPayLoadSize_P("Input/unprotector.pdf",0,""),
    GetPayLoadSize_P("Input/Protector/customerTemplate.pdf",26488,""),
    GetPayLoadSize_P("Input/Protector/V1V1.pdf",55728,""),
    GetPayLoadSize_P("Input/Protector/anyone.pdf",25699,"")));

TEST_P(PDFWrapperDoc_GetPayloadFileName,GetPayloadFileName_T) {
  GetPayloadFileName_P TParam = GetParam();
  pdfobjectmodel::PDFModuleMgr::Initialize();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;

  auto inFile = std::make_shared<std::fstream>(fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::shared_ptr<std::iostream> input_encrypted_IO = inFile;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);
  bool ret;
  std::wstring file_name;
  try {
    std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =  pdfobjectmodel::PDFWrapperDoc::Create(input_encrypted);
    ret = pdf_wrapper_doc->GetPayloadFileName(file_name);
  }
  catch(const rmsauth::Exception& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionMess, message);
    return;
  }
  EXPECT_EQ(TParam.ret, ret);
  if (TParam.ret == true) {
    EXPECT_EQ(TParam.file_name, file_name);
  }
}
INSTANTIATE_TEST_CASE_P(,PDFWrapperDoc_GetPayloadFileName,testing::Values(
    GetPayloadFileName_P("Input/Protector/customerTemplate.pdf",L"",true,""),
    GetPayloadFileName_P("Input/Protector/OfficeTemplate.ppdf",L"",true,""),
    GetPayloadFileName_P("Input/Protector/password.pdf",L"",true,""),
    GetPayloadFileName_P("Input/Protector/test.pdf",L"Test",true,""),
    GetPayloadFileName_P("Input/Protector/V1V1.pdf",L"",true,""),//V1
    GetPayloadFileName_P("Input/Protector/cer.pdf",L"",true,""),
    GetPayloadFileName_P("Input/Protector/anyone.pdf",L"MicrosoftIRMServices Protected PDF.pdf",true,"")));

TEST_P(PDFUnencryptedWrapperCreator_CreateUnencryptedWrapper,CreateUnencryptedWrapper_T) {
  CreateUnencryptedWrapper_P TParam = GetParam();
  pdfobjectmodel::PDFModuleMgr::Initialize();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;
  auto inFile = std::make_shared<std::fstream>(fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::string file_out = "OutPut/CreateUnencryptedWrapper/anyone.pdf";
  auto outFile = std::make_shared<std::fstream>(file_out, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
  std::shared_ptr<std::iostream> input_encrypted_IO = inFile;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);
  uint32_t ret;
  try {
    std::unique_ptr<pdfobjectmodel::PDFUnencryptedWrapperCreator> pdf_wrapper_doc =  pdfobjectmodel::PDFUnencryptedWrapperCreator::Create(input_encrypted);
    std::shared_ptr<std::iostream> input_encrypted_IO = outFile;
    auto output_stream = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);
    ret = pdf_wrapper_doc->CreateUnencryptedWrapper(output_stream);
  }
  catch(const rmsauth::Exception& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionMess, message);
    return;
  }
  EXPECT_EQ(TParam.ret, ret);
};
INSTANTIATE_TEST_CASE_P(,PDFUnencryptedWrapperCreator_CreateUnencryptedWrapper,testing::Values(
    CreateUnencryptedWrapper_P("Input/XFAStatic.pdf",1,""),
    CreateUnencryptedWrapper_P("Input/XFADyanmic-crash.pdf",1,""),
    CreateUnencryptedWrapper_P("Input/sign.pdf",1,""),
    CreateUnencryptedWrapper_P("Input/Protector/pwd_protect.pdf",1,""),
    CreateUnencryptedWrapper_P("Input/ERROR/Error.pdf",0,""),
    CreateUnencryptedWrapper_P("Input/Protector/cer.pdf",0,""),
    CreateUnencryptedWrapper_P("Input/Protector/password.pdf",0,""),
    CreateUnencryptedWrapper_P("Input/Protector/customerTemplate.pdf",1,""),
    CreateUnencryptedWrapper_P("Input/Protector/V1V1.pdf",1,""),
    CreateUnencryptedWrapper_P("Input/Protector/OfficeTemplate.ppdf",0,""),
    CreateUnencryptedWrapper_P("Input/unprotector.pdf",1,""),
    CreateUnencryptedWrapper_P("Input/demage.pdf",0,""),
    CreateUnencryptedWrapper_P("Input/Test.txt",0,"")));

void PDFUnencryptedWrapperCreator_SetPayloadInfo::SetUpTestCase() {
  SetUserPolicy();
}

void PDFUnencryptedWrapperCreator_SetPayloadInfo::TearDownTestCase() {

}

TEST_P(PDFUnencryptedWrapperCreator_SetPayloadInfo, SetPayloadInfo_T) {
  SetPayloadInfo_P TParam= GetParam();
  std::string fileIn=unittests::dependency::GetCurrentInputFile() +"Input/unprotector.pdf";


  auto inFile = std::make_shared<std::fstream>(
      fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  std::unique_ptr<pdfobjectmodel::PDFCreator> pdf_creator_ = pdfobjectmodel::PDFCreator::Create();

  auto encryptedSS = std::make_shared<std::stringstream>();
  std::shared_ptr<std::iostream> encryptedIOS = encryptedSS;
  auto output_encrypted = rmscrypto::api::CreateStreamFromStdStream(encryptedIOS);

  std::string originalFileExtension = ".pFile";

  auto p_PDFprotector = std::make_shared<fileapi::PDFProtector_unit>(fileIn, originalFileExtension, inFile);
  auto crypto_hander = std::make_shared<fileapi::PDFCryptoHandler_child>(p_PDFprotector);

  p_PDFprotector->SetUserPolicy(user_policy_);

  std::string filter_name = PDF_PROTECTOR_FILTER_NAME;
  std::vector<unsigned char> publishing_license = user_policy_->SerializedPolicy();

  uint32_t ret;
  ret = pdf_creator_->CreateCustomEncryptedFile(
      fileIn,
      filter_name,
      publishing_license,
      crypto_hander,
      output_encrypted);

  std::string wrapper_in= unittests::dependency::GetCurrentInputFile() +"Input/wrapper.pdf";


  auto inwrapper = std::make_shared<std::fstream>(wrapper_in,
                                                  std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::shared_ptr<std::iostream> input_wrapper_IO = inwrapper;
  auto input_wrapper = rmscrypto::api::CreateStreamFromStdStream(input_wrapper_IO);
  try {
    std::unique_ptr<pdfobjectmodel::PDFUnencryptedWrapperCreator> pdf_wrapper_creator_ = pdfobjectmodel::PDFUnencryptedWrapperCreator::Create(input_wrapper);
    pdf_wrapper_creator_ = pdfobjectmodel::PDFUnencryptedWrapperCreator::Create(input_wrapper);
    pdf_wrapper_creator_->SetPayloadInfo(
        TParam.sub_type,
        TParam.file_name,
        TParam.description,
        TParam.version_num);
    pdf_wrapper_creator_->SetPayLoad(output_encrypted);

    std::string file_out =unittests::dependency::GetCurrentInputFile()+ TParam.fileout;
    auto output_stream = std::make_shared<std::fstream>(
        file_out, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    std::shared_ptr<std::iostream> outputIO = output_stream;
    auto output_wrapper = rmscrypto::api::CreateStreamFromStdStream(outputIO);
    pdf_wrapper_creator_->CreateUnencryptedWrapper(output_wrapper);
    //**************************
    auto inpro = std::make_shared<std::fstream>(file_out, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    std::shared_ptr<std::iostream> input_encrypted_IO = inpro;
    auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);
    bool ret;
    std::wstring file_name;
    std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =  pdfobjectmodel::PDFWrapperDoc::Create(input_encrypted);
    pdf_wrapper_doc->GetPayloadFileName(file_name);

    std::wstring graphic_filter;
    float version_num;
    pdf_wrapper_doc->GetCryptographicFilter(graphic_filter, version_num);
    if (!(TParam.file_name.compare(L" "))) {
        EXPECT_EQ(TParam.sub_type,graphic_filter);
        EXPECT_EQ(L"Embedded File",file_name);
        EXPECT_EQ(TParam.version_num, version_num);
    } else {
      EXPECT_EQ(TParam.sub_type, graphic_filter);
      EXPECT_EQ(TParam.file_name, file_name);
      EXPECT_EQ(TParam.version_num, version_num);
    }

  }
  catch (const rmsauth::Exception& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }
  catch (const rmscore::exceptions::RMSPDFFileException& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }
  catch (const rmscore::exceptions::RMSException& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }

}
INSTANTIATE_TEST_CASE_P(,PDFUnencryptedWrapperCreator_SetPayloadInfo,testing::Values(
    SetPayloadInfo_P(L" ",L" ",L" ","/OutPut/SetPayloadInfo/nullvalue.pdf",1,""),//
    SetPayloadInfo_P(L"test",L"test",L"test","/OutPut/SetPayloadInfo/China.pdf",1,""),
    SetPayloadInfo_P(L"MicrosoftIRMServices",L"MicrosoftIRMServices",L"This embedded file is encrypted using MicrosoftIRMServices filter","/OutPut/SetPayloadInfo/normal.pdf",2,""),
    SetPayloadInfo_P(L"Test",L"Test",L"Test","/OutPut/SetPayloadInfo/test.pdf",7,"")));
