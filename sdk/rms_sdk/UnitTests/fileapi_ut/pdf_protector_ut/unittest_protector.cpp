#include "unittest_protector.h"
#include "Auth.h"
#include "depend.h"

//****************WithWrapperCreate Test***************************************************

TEST_P(ProtectorWithWrapper_Create,Create_T) {
  Create_P TParam = GetParam();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;
  auto inFile = std::make_shared<std::fstream>(
      fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  std::string filename = fileIn;
  std::string new_file_name = "";
  std::unique_ptr<rmscore::fileapi::ProtectorWithWrapper> obj =NULL;
  try {
    obj = rmscore::fileapi::ProtectorWithWrapper::Create(filename, inFile, new_file_name);
  }
  catch(const rmscore::exceptions::RMSException& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }
  EXPECT_EQ(TParam.ret, obj != NULL);
  EXPECT_EQ(TParam.outputFileName, new_file_name);
}
#ifdef Q_OS_WIN32
INSTANTIATE_TEST_CASE_P(, ProtectorWithWrapper_Create, testing::Values(
      Create_P("","","The input stream is invalid",false),
      Create_P("Input/demage.pdf","demage.pdf","",true),
      Create_P("Input/test.txt","","This file format is not supported.",true),
      Create_P("Input/Error.pdf","","The input stream is invalid",false),
      Create_P("Input/Protector/MIPProtector.pdf","MIPProtector.pdf","NO Exception",true),
      Create_P("Input/Protector/customerTemplate.pdf","customerTemplate.pdf","",true),
      Create_P("Input/Protector/OfficeTemplate.ppdf","OfficeTemplate.pdf","This file format is not supported.",false),//4
      Create_P("Input/Protector/password.pdf","password.pdf","",true),
      //Create_P("Input/Protector/cer2-no.pdf","cer2-no.pdf","",true),
      Create_P("Input/unprotector.pdf","unprotector.pdf","",true)
));
#endif
#ifdef Q_OS_LINUX
INSTANTIATE_TEST_CASE_P(,ProtectorWithWrapper_Create,testing::Values(
    //Create_P("Input/test.txt","","This file format is not supported.",false),
    Create_P("","","The input stream is invalid",false),
    Create_P("Input/demage.pdf","demage.pdf","",true),
    Create_P("Input/test.txt","","The input stream is invalid",false),
    Create_P("Input/Error.pdf","","The input stream is invalid",false),
    Create_P("Input/Protector/MIPProtector.pdf","MIPProtector.pdf","NO Exception",true),
    Create_P("Input/Protector/customerTemplate.pdf","customerTemplate.pdf","",true),
    Create_P("Input/Protector/OfficeTemplate.ppdf","OfficeTemplate.pdf","This file format is not supported.",false),//4
    Create_P("Input/Protector/password.pdf","password.pdf","",true),
    //Create_P("Input/Protector/cer2-no.pdf","cer2-no.pdf","",true),
    Create_P("Input/unprotector.pdf","unprotector.pdf","",true)
));
#endif

//****************IsProtected Test***************************************************

TEST_P(ProtectorWithWrapper_IsProtected,IsProtected_T) {
  IsProtected_P TParam = GetParam();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;

  auto inFile = std::make_shared<std::fstream>(
      fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  std::string filename = fileIn;//fileIn.substr(fileIn.find_last_of("\\/") + 1);
  std::string new_file_name = "";
  bool ret;
  try {
    std::unique_ptr<rmscore::fileapi::ProtectorWithWrapper> obj =
        rmscore::fileapi::ProtectorWithWrapper::Create(filename, inFile, new_file_name);
    ret=obj->IsProtected();
  }
  catch(const rmscore::exceptions::RMSException& e) {
    std::string message(e.what());
    EXPECT_EQ(TParam.ExceptionsMess, message);
    return;
  }

  EXPECT_EQ(TParam.ret, ret);
}
INSTANTIATE_TEST_CASE_P(,ProtectorWithWrapper_IsProtected,testing::Values(
    IsProtected_P("Input/unprotector.pdf","NO Exception",false),
    IsProtected_P("Input/Protector/MIPProtector.pdf","NO Exception",true),
    IsProtected_P("Input/Protector/customerTemplate.pdf","NO Exception",false),
    IsProtected_P("Input/Protector/V1V1.pdf","NO Exception",true),
    IsProtected_P("Input/Protector/V1NoWrapper.pdf","NO Exception",true),
    IsProtected_P("Input/Protector/V1NoWrapper.pdf","NO Exception",true)
));

//********************protector***********************************************

TEST_P(ProtectorWithWrapper_Unprotector,Unprotector_T) {
  Unprotector_P TParam = GetParam();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;
  auto inFile = std::make_shared<std::fstream>(fileIn, std::ios_base::in | std::ios_base::binary);
  std::string filename = fileIn;
  std::string new_file_name = "";
  try {
      std::unique_ptr<rmscore::fileapi::ProtectorWithWrapper> obj =
            rmscore::fileapi::ProtectorWithWrapper::Create(filename, inFile, new_file_name);
      std::string file_out = unittests::dependency::GetCurrentInputFile() + TParam.fileout;

    // create streams
      auto outFile = std::make_shared<std::fstream>(
                  file_out, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    AuthCallback auth(CLIENTID, REDIRECTURL);
    ConsentCallback consent;
    std::shared_ptr<std::atomic<bool> > cancelState(new std::atomic<bool>(false));
    std::shared_ptr<std::atomic<bool> > cancelState2(new std::atomic<bool>(true));
    rmscore::fileapi::UnprotectOptions upt (false, true);
    rmscore::fileapi::UserContext ut (CLIENTEMAIL, auth, consent);

    if (TParam.cancelState) cancelState = cancelState2;
    obj->Unprotect(ut, upt, outFile, cancelState);

    inFile = std::make_shared<std::fstream>(file_out, std::ios_base::in | std::ios_base::binary);
    std::unique_ptr<rmscore::fileapi::ProtectorWithWrapper> obj2 =
          rmscore::fileapi::ProtectorWithWrapper::Create(file_out, inFile, new_file_name);
    bool ret=obj2->IsProtected();
    EXPECT_EQ(ret, false);
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
INSTANTIATE_TEST_CASE_P(,ProtectorWithWrapper_Unprotector,testing::Values(
    Unprotector_P("Input/Protector/Protected package.pdf","OutPut/unProtected/Protected package.pdf","NO Exception",false),
    Unprotector_P("Input/Protector/demage.pdf","OutPut/unProtected/demage.pdf","It is not a valid RMS-protected file.",false),
    Unprotector_P("Input/XFAStatic.pdf","OutPut/unProtected/Protected XFAStatic.pdf","It is not a valid RMS-protected file.",false),
    Unprotector_P("Input/Protector/Protected XFAStatic.pdf","OutPut/unProtected/Protected XFAStatic.pdf","NO Exception",false),
    Unprotector_P("Input/XFADyanmic-crash.pdf","OutPut/unProtected/XFADyanmic-crash.pdf","It is not a valid RMS-protected file.",false),
    Unprotector_P("Input/sign.pdf","OutPut/unProtected/Protected sign.pdf","It is not a valid RMS-protected file.",false),
    Unprotector_P("Input/Protector/cer.pdf","OutPut/unProtected/cer.pdf","It is not a valid RMS-protected file.",false),
    Unprotector_P("Input/Protector/MIPProtector.pdf","OutPut/unProtected/MIPProtector_true.pdf","Network operation was cancelled by user",true),
    Unprotector_P("Input/Protector/MIPProtector.pdf","OutPut/unProtected/MIPProtector.pdf","",false),
    Unprotector_P("Input/Protector/phantomOfficeT.pdf","OutPut/unProtected/phantomOfficeT.pdf","Only the owner has the right to unprotect the document.",false),
    Unprotector_P("Input/unprotector.pdf","OutPut/unProtected/unprotector.pdf","It is not a valid RMS-protected file.",false),
    Unprotector_P("Input/Protector/password.pdf","OutPut/unProtected/password.pdf","It is not a valid RMS-protected file.",false),
    Unprotector_P("Input/Protector/attachment.pdf","OutPut/unProtected/attachment.pdf","",false),
    Unprotector_P("Input/Protector/anyone.pdf","OutPut/error/errorPath.pdf","Output stream invalid",false),
    //Unprotector_P("Input/Protector/cer2-no.pdf","OutPut/unProtected/cer2-no.pdf","It is not a valid RMS-protected file.",false),
    Unprotector_P("Input/Protector/V1V1.pdf","OutPut/unProtected/V1V1.pdf","",false),
    Unprotector_P("Input/Protector/V2NoWrapper.pdf","OutPut/unProtected/V2NoWrapper.pdf","",false),
    Unprotector_P("Input/Protector/V1NoWrapper.pdf","OutPut/unProtected/V1NoWrapper.pdf","",false),
    Unprotector_P("Input/Protector/V1V0.pdf","OutPut/unProtected/V1V0.pdf","",false)
));
//************ProtectWithTemplate********************************************

TEST_P(ProtectorWithWrapper_ProtectWithTemplate,ProtectWithTemplate_T) {
  ProtectWithTemplate_P TParam = GetParam();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;
  std::string wrapper_in =unittests::dependency::GetCurrentInputFile() + "Input/wrapper.pdf";
  auto inFile = std::make_shared<std::fstream>(
      fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  std::string filename = fileIn;
  std::string new_file_name = "";
  std::string file_out= unittests::dependency::GetCurrentInputFile() + TParam.fileout;
  auto outFile = std::make_shared<std::fstream>(
      file_out, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
  try {
    std::unique_ptr<rmscore::fileapi::ProtectorWithWrapper> obj =
            rmscore::fileapi::ProtectorWithWrapper::Create(filename, inFile, new_file_name);

    AuthCallback auth(CLIENTID, REDIRECTURL);
    ConsentCallback consent;
    rmscore::modernapi::AppDataHashMap signedData;
    std::shared_ptr<std::atomic<bool> > cancelState(new std::atomic<bool>(false));
    std::shared_ptr<std::atomic<bool> > cancelState2(new std::atomic<bool>(true));
    auto templates_future = rmscore::modernapi::TemplateDescriptor::GetTemplateListAsync(
        CLIENTEMAIL, auth, std::launch::deferred, cancelState);

    auto templates = templates_future.get();

    rmscore::fileapi::ProtectWithTemplateOptions pt (rmscore::fileapi::CryptoOptions::AUTO,
                                                     (*templates)[0], signedData, true);

    rmscore::fileapi::UserContext ut (CLIENTEMAIL, auth, consent);

    auto inWrapper = std::make_shared<std::fstream>(
        wrapper_in, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

    if(TParam.cancelState) cancelState=cancelState2;

    obj->SetWrapper(inWrapper);
    obj->ProtectWithTemplate(ut, pt, outFile, cancelState);
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
  outFile = std::make_shared<std::fstream>(
      file_out, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::unique_ptr<rmscore::fileapi::Protector> outobj = rmscore::fileapi::Protector::Create(
      file_out,
      outFile,
      filename);
  bool ret = false;
  ret= outobj->IsProtected();
  EXPECT_EQ(TParam.ret, ret);
}
INSTANTIATE_TEST_CASE_P(,ProtectorWithWrapper_ProtectWithTemplate,testing::Values(
    ProtectWithTemplate_P("Input/package.pdf","OutPut/protectorWithTemplate/package.pdf","NO Exception",false,true),
    ProtectWithTemplate_P("Input/demage.pdf","OutPut/protectorWithTemplate/demage.pdf","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithTemplate_P("Input/unprotector.pdf","OutPut/protectorWithTemplate/canscelstatetrue.pdf","Network operation was cancelled by user",true,false),
    ProtectWithTemplate_P("Input/unprotector.pdf","OutPut/protectorWithTemplate/unprotector.pdf","NO Exception",false,true),
    ProtectWithTemplate_P("Input/XFAStatic.pdf","OutPut/protectorWithTemplate/XFAStatic.pdf","NO Exception",false,true),
    ProtectWithTemplate_P("Input/sign.pdf","OutPut/protectorWithTemplate/Protected sign.pdf","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithTemplate_P("Input/XFADyanmic-crash.pdf","OutPut/protectorWithTemplate/Protected XFADyanmic-crash.pdf","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithTemplate_P("Input/XFADyanmic.pdf","OutPut/protectorWithTemplate/Protected XFADyanmic.pdf","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithTemplate_P("Input/Protector/password.pdf","OutPut/protectorWithTemplate/password.pdf","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithTemplate_P("Input/Protector/MaxOwner.pdf","OutPut/protectorWithTemplate/MaxOwner.pdf","File is already protected",false,false),
    ProtectWithTemplate_P("Input/Protector/V2NoWrapper.pdf","OutPut/protectorWithTemplate/V2NoWrapper.pdf","File is already protected",false,false),
    ProtectWithTemplate_P("Input/Protector/V1NoWrapper.pdf","OutPut/protectorWithTemplate/V1NoWrapper.pdf","File is already protected",false,false),
    ProtectWithTemplate_P("Input/Protector/V1V1.pdf","OutPut/protectorWithTemplate/V1V1.pdf","File is already protected",false,false),
    ProtectWithTemplate_P("Input/Protector/customerTemplate.pdf","OutPut/protectorWithTemplate/customerTemplate.pdf","File is already protected",false,true),
    ProtectWithTemplate_P("Input/unprotector.pdf","OutPut/error/canscelstatetrue.pdf","Output stream invalid",false,false),
    ProtectWithTemplate_P("Input/Protector/cer.pdf","OutPut/protectorWithTemplate/cer.pdf","Failed to encrypt the file. The file is invalid.",false,true),
    // ProtectWithTemplate_P("Input/Protector/cer2-no.pdf","OutPut/protectorWithTemplate/cer2-no.pdf","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithTemplate_P("Input/Protector/pwd_protect.pdf","OutPut/protectorWithTemplate/pwd_protect.pdf","Failed to encrypt the file. The file is invalid.",false,false)
));

//************ProtectWithTemplate********************************************

TEST_P(ProtectorWithWrapper_ProtectWithCustomRights, ProtectWithCustomRights_T) {
  ProtectWithCustomRights_P TParam = GetParam();
  std::string fileIn = unittests::dependency::GetCurrentInputFile() + TParam.fileIn;
  std::string wrapper_in =unittests::dependency::GetCurrentInputFile() + "Input/wrapper.pdf";;
  auto inFile = std::make_shared<std::fstream>(
      fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  std::string filename = fileIn;
  std::string new_file_name = "";
  std::unique_ptr<rmscore::fileapi::ProtectorWithWrapper> obj =
          rmscore::fileapi::ProtectorWithWrapper::Create(filename, inFile, new_file_name);
  std::string file_out = unittests::dependency::GetCurrentInputFile() + TParam.fileout;
  auto outFile = std::make_shared<std::fstream>(
      file_out, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
  //auto self = shared_from_this();
  AuthCallback auth(CLIENTID, REDIRECTURL);
  ConsentCallback consent;
  std::shared_ptr<std::atomic<bool> > cancelState(new std::atomic<bool>(false));
  std::shared_ptr<std::atomic<bool> > cancelState2(new std::atomic<bool>(true));
  std::string userlists = TParam.userlists;
  std::string rightlists = TParam.RightList;
  auto endvalidation = std::chrono::system_clock::now() + std::chrono::hours(480);
  std::vector<rmscore::modernapi::UserRights> userRights;
  for(;;) {
    rmscore::modernapi::UserList  users;
    rmscore::modernapi::RightList rights;
    int upos=userlists.find_first_of("|");
    int rpos=rightlists.find_first_of("|");
    if (upos==-1||rpos==-1) break;
    std::string user =userlists.substr(0,upos);

    std::string right =rightlists.substr(0,rpos);
    for(;;) {
      int rrpos=right.find_first_of(" ");
      if (rrpos==-1) break;
      std::string rright=right.substr(0,rrpos);
      rights.push_back(rright);
      int rrlen=right.length();
      right = right.substr(rrpos+1,rrlen-1);
    }
    users.push_back(user);
    int ulen=userlists.length();
    int rlen=rightlists.length();
    userlists = userlists.substr(upos+1,ulen-1);
    rightlists = rightlists.substr(rpos+1,rlen-1);
    userRights.push_back(rmscore::modernapi::UserRights(users   , rights));
  }
  rmscore::modernapi::PolicyDescriptor desc(userRights);
  desc.Referrer(std::make_shared<std::string>("https://client.test.app"));
  desc.ContentValidUntil(endvalidation);
  desc.AllowOfflineAccess(false);
  desc.Name("Test Name");
  desc.Description("Test Description");

  rmscore::fileapi::ProtectWithCustomRightsOptions pc (rmscore::fileapi::CryptoOptions::AES128_ECB,
                                                       desc, true);
  //*********************

  rmscore::fileapi::UserContext ut (CLIENTEMAIL, auth, consent);

  auto inWrapper = std::make_shared<std::fstream>(
      wrapper_in, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  obj->SetWrapper(inWrapper);
  if (TParam.cancelState) cancelState = cancelState2;
  try {
    obj->ProtectWithCustomRights(ut,pc,outFile, cancelState);
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
  outFile = std::make_shared<std::fstream>(
      file_out, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::unique_ptr<rmscore::fileapi::Protector> outobj =
      rmscore::fileapi::Protector::Create(file_out, outFile, filename);

  bool ret = false;
  ret= outobj->IsProtected();
  EXPECT_EQ(TParam.ret, ret);
}

INSTANTIATE_TEST_CASE_P(,ProtectorWithWrapper_ProtectWithCustomRights,testing::Values(
    ProtectWithCustomRights_P("Input/package.pdf","OutPut/protectWithCustomeRight/package.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW |","NO Exception",false,true),
    ProtectWithCustomRights_P("Input/demage.pdf","OutPut/protectWithCustomeRight/demage.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW |","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithCustomRights_P("Input/XFAStatic.pdf","OutPut/protectWithCustomeRight/XFAStatic.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW |","NO Exception",false,true),
    ProtectWithCustomRights_P("Input/sign.pdf","OutPut/protectWithCustomeRight/Protected sign.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW |","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithCustomRights_P("Input/XFADyanmic-crash.pdf","OutPut/protectWithCustomeRight/Protected XFADyanmic-crash.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW |","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithCustomRights_P("Input/XFADyanmic.pdf","OutPut/protectWithCustomeRight/Protected XFADyanmic.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW |","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithCustomRights_P("Input/unprotector.pdf","OutPut/protectWithCustomeRight/canclestatetrue.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","Network operation was cancelled by user",true,false),
    ProtectWithCustomRights_P("Input/Protector/pwd_protect.pdf","OutPut/protectWithCustomeRight/pwd_protect.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","Failed to encrypt the file. The file is invalid.",false,false),
    //ProtectWithCustomRights_P("Input/Protector/cer2-no.pdf","OutPut/protectWithCustomeRight/cer2-no.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithCustomRights_P("Input/Protector/cer.pdf","OutPut/protectWithCustomeRight/cer.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","Failed to encrypt the file. The file is invalid.",false,true),
    ProtectWithCustomRights_P("Input/unprotector.pdf","OutPut/protectWithCustomeRight/Owner.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|appstore@foxitsoftwareinc.onmicrosoft.com|tom@foxitsoftwareinc.onmicrosoft.com|","OWNER |EDIT EXPORT |VIEW PRINT |","",false,true),
    //ProtectWithCustomRights_P("Input/unprotector.pdf","OutPut/protectWithCustomeRight/TwoUsers.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|appstore@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |VIEW PRINT |","NO Exception",false,true),
    ProtectWithCustomRights_P("Input/unprotector.pdf","OutPut/protectWithCustomeRight/invalidEmail.pdf","XXX|  ","VIEW EDIT |","Invalid PL",false,false),
    ProtectWithCustomRights_P("Input/unprotector.pdf","OutPut/protectWithCustomeRight/invaliduser.pdf","XXX@qq.com|  ","VIEW EDIT |","NO Exception",false,true),
    ProtectWithCustomRights_P("Input/unprotector.pdf","OutPut/protectWithCustomeRight/ProtectorWithUserIsnull.pdf","","VIEW EDIT |EXPORT VIEW PRINT |","Got an invalid response from the server : args are empty.",false,false),
    ProtectWithCustomRights_P("Input/unprotector.pdf","OutPut/protectWithCustomeRight/anyone.pdf","Anyone|  ","EXPORT VIEW PRINT |","NO Exception",false,true),
    ProtectWithCustomRights_P("Input/unprotector.pdf","OutPut/protectWithCustomeRight/InvalidRight.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","XXXX |","No Exception",false,true),
    ProtectWithCustomRights_P("Input/unprotector.pdf","OutPut/protectWithCustomeRight/RightisNULL.pdf","zhq@foxitsoftwareinc.onmicrosoft.com","","Got an invalid response from the server : args are empty.",false,false),
    ProtectWithCustomRights_P("Input/Protector/password.pdf","OutPut/protectWithCustomeRight/password.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","Failed to encrypt the file. The file is invalid.",false,false),
    ProtectWithCustomRights_P("Input/Protector/MaxOwner.pdf","OutPut/protectWithCustomeRight/MaxOwner.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","File is already protected",false,false),
    ProtectWithCustomRights_P("Input/Protector/V2NoWrapper.pdf","OutPut/protectWithCustomeRight/V2NoWrapper.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","File is already protected",false,false),
    ProtectWithCustomRights_P("Input/Protector/V1NoWrapper.pdf","OutPut/protectWithCustomeRight/V1NoWrapper.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","File is already protected",false,false),
    ProtectWithCustomRights_P("Input/Protector/customerTemplate.pdf","OutPut/protectWithCustomeRight/customerTemplate.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","NO Exception",false,true)
    //ProtectWithCustomRights_P("Input/Protector/phantomOfficeT.pdf","OutPut/protectWithCustomeRight/phantomOfficeT.pdf","zhq@foxitsoftwareinc.onmicrosoft.com|","VIEW EDIT |","File is already protected",false,false)
));

TEST_P(ProtectorWithWrapper_SetWrapper,SetWrapper_ProtectWithTemplate) {
  SetWrapper_P TParam = GetParam();
  std::string wrapper_in = unittests::dependency::GetCurrentInputFile() + TParam.wrapperin;
  std::string fileIn=unittests::dependency::GetCurrentInputFile() + "Input/unprotector.pdf";
  auto inFile = std::make_shared<std::fstream>(
      fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::string filename = fileIn;
  std::string new_file_name = "";
  std::unique_ptr<rmscore::fileapi::ProtectorWithWrapper> obj =
      rmscore::fileapi::ProtectorWithWrapper::Create(filename, inFile, new_file_name);
  std::string file_out = unittests::dependency::GetCurrentInputFile() + TParam.outfile;
  auto outFile = std::make_shared<std::fstream>(
      file_out, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
  //auto self = shared_from_this();

  AuthCallback auth(CLIENTID, REDIRECTURL);
  ConsentCallback consent;
  rmscore::modernapi::AppDataHashMap signedData;
  std::shared_ptr<std::atomic<bool> > cancelState(new std::atomic<bool>(false));
  auto templates_future = rmscore::modernapi::TemplateDescriptor::GetTemplateListAsync(
      CLIENTEMAIL, auth, std::launch::deferred, cancelState);

  auto templates = templates_future.get();

  rmscore::fileapi::ProtectWithTemplateOptions pt (rmscore::fileapi::CryptoOptions::AES128_ECB,
                                                   (*templates)[0], signedData, true);

  rmscore::fileapi::UserContext ut (CLIENTEMAIL, auth, consent);

  auto inWrapper = std::make_shared<std::fstream>(
      wrapper_in, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  try {
    int len = TParam.wrapperin.length();
    if (len<1) {
      obj->ProtectWithTemplate(ut, pt, outFile, cancelState);
    } else {
      obj->SetWrapper(inWrapper);
      obj->ProtectWithTemplate(ut, pt, outFile, cancelState);
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
  outFile = std::make_shared<std::fstream>(
      file_out, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::unique_ptr<rmscore::fileapi::Protector> outobj =
      rmscore::fileapi::Protector::Create(file_out, outFile, filename);

  bool ret = false;
  ret= outobj->IsProtected();
  EXPECT_EQ(TParam.ret, ret);

}

TEST_P(ProtectorWithWrapper_SetWrapper, SetWrapper_ProtectWithCustomRights) {
  SetWrapper_P TParam = GetParam();
  std::string wrapper_in = unittests::dependency::GetCurrentInputFile() + TParam.wrapperin;
  std::string fileIn=unittests::dependency::GetCurrentInputFile() + "Input/unprotector.pdf";
  auto inFile = std::make_shared<std::fstream>(
      fileIn, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::string filename = fileIn;
  std::string new_file_name = "";
  std::unique_ptr<rmscore::fileapi::ProtectorWithWrapper> obj =
      rmscore::fileapi::ProtectorWithWrapper::Create(filename, inFile, new_file_name);
  std::string file_out = unittests::dependency::GetCurrentInputFile() + TParam.outfile;
  auto outFile = std::make_shared<std::fstream>(
      file_out, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
  //auto self = shared_from_this();

  AuthCallback auth(CLIENTID, REDIRECTURL);
  ConsentCallback consent;
  std::shared_ptr<std::atomic<bool> > cancelState(new std::atomic<bool>(false));
  std::shared_ptr<std::atomic<bool> > cancelState2(new std::atomic<bool>(true));
  auto endvalidation = std::chrono::system_clock::now() + std::chrono::hours(480);
  std::vector<rmscore::modernapi::UserRights> userRights;
  rmscore::modernapi::UserList  users;
  rmscore::modernapi::RightList rights;
  users.push_back(CLIENTEMAIL);
  rights.push_back("VIEW");
  userRights.push_back(rmscore::modernapi::UserRights(users   , rights));
  rmscore::modernapi::PolicyDescriptor desc(userRights);
  desc.Referrer(std::make_shared<std::string>("https://client.test.app"));
  desc.ContentValidUntil(endvalidation);
  desc.AllowOfflineAccess(false);
  desc.Name("Test Name");
  desc.Description("Test Description");

  rmscore::fileapi::ProtectWithCustomRightsOptions pc (rmscore::fileapi::CryptoOptions::AES128_ECB,
                                                       desc, true);
  //*********************

  rmscore::fileapi::UserContext ut (CLIENTEMAIL, auth, consent);//此处的Email并没有什么用处，

  auto inWrapper = std::make_shared<std::fstream>(
      wrapper_in, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  try {
    int len = TParam.wrapperin.length();
    if (len<1) {
      obj->ProtectWithCustomRights(ut,pc,outFile, cancelState);
    } else {
      obj->SetWrapper(inWrapper);
      obj->ProtectWithCustomRights(ut,pc,outFile, cancelState);
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
  outFile = std::make_shared<std::fstream>(
      file_out, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  std::unique_ptr<rmscore::fileapi::Protector> outobj =
      rmscore::fileapi::Protector::Create(file_out, outFile, filename);

  bool ret = false;
  ret = outobj->IsProtected();
  EXPECT_EQ(TParam.ret, ret);
}

INSTANTIATE_TEST_CASE_P(,ProtectorWithWrapper_SetWrapper,testing::Values(
    SetWrapper_P("Input/XFAStatic.pdf","OutPut/setwrapper/XFAStatic.pdf","NO Exception",true),
    SetWrapper_P("Input/XFADyanmic-crash.pdf","OutPut/setwrapper/XFADyanmic-crash.pdf","NO Exception",true),
    SetWrapper_P("Input/sign.pdf","OutPut/setwrapper/sign.pdf","NO Exception",true),
    SetWrapper_P("Input/package.pdf","OutPut/setwrapper/package.pdf","",true),
    SetWrapper_P("","OutPut/setwrapper/SetWrappererror.pdf","Not set the input wrapper stream.",false),
    SetWrapper_P("Input/2Wrapper.pdf","OutPut/setwrapper/SetWrappererror.pdf","Failed to create PDF IRM V2 file. The wrapper doc may be invalid.",false),
    SetWrapper_P("Input/wrapper.pdf","OutPut/setwrapper/SetWrappernormal.pdf","NO Exception",true),
    SetWrapper_P("Input/Protector/password.pdf","OutPut/setwrapper/SetWrapperpassword.pdf","Failed to create PDF IRM V2 file. The wrapper doc may be invalid.",false),
    SetWrapper_P("Input/Protector/MIPProtector.pdf","OutPut/setwrapper/MIPProtector.pdf","NO Exception",true),
    SetWrapper_P("Input/demage.pdf","OutPut/setwrapper/demage.pdf","Failed to create PDF IRM V2 file. The wrapper doc may be invalid.",false),
    SetWrapper_P("Input/123.txt","OutPut/setwrapper/SetWrappertxt.pdf","Failed to create PDF IRM V2 file. The wrapper doc may be invalid.",false),
    SetWrapper_P("Input/Protector/cer.pdf","OutPut/setwrapper/cer.pdf","Failed to create PDF IRM V2 file. The wrapper doc may be invalid.",false)
));
