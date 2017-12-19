#ifndef RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_UNITTEST_PDFOBJECTMODEL_H_
#define RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_UNITTEST_PDFOBJECTMODEL_H_

#include "depend.h"
#include <string>
#include <memory>

struct CreateCustomEncryptedFile_P {
  CreateCustomEncryptedFile_P(
      std::string fileIn_,
      std::string fileout_,
      std::string filterName_,
      std::string ExceptionsMess_,
      uint32_t ret_) {
    fileIn = fileIn_;
    fileout =fileout_;
    filter_name = filterName_;
    ret = ret_;
    ExceptionsMess=ExceptionsMess_;
  }
  std::string fileIn;
  std::string fileout;
  std::string filter_name;
  std::string ExceptionsMess;
  uint32_t ret;
};

class PDFCreator_CreateCustomEncryptedFile : public ::testing::TestWithParam<CreateCustomEncryptedFile_P> {
 public:
  static void SetUpTestCase();
  static void TearDownTestCase();
};

struct UnprotectCustomEncryptedFile_P {
  UnprotectCustomEncryptedFile_P(
      std::string fileIn_,
      std::string fileout_,
      std::string filterName_,
      std::string Exceptionmess_,
      uint32_t ret_) {
    fileIn = fileIn_;
    fileout =fileout_;
    filter_name = filterName_;
    ExceptionsMess = Exceptionmess_;
    ret = ret_;

  }
  std::string fileIn;
  std::string fileout;
  std::string filter_name;
  std::string ExceptionsMess;
  uint32_t ret;
};

class PDFCreator_UnprotectCustomEncryptedFile:public ::testing::TestWithParam<UnprotectCustomEncryptedFile_P> {
 public:
   static void SetUpTestCase();
   static void TearDownTestCase();
};

struct GetWrapperType_P {
  GetWrapperType_P(
      std::string fileIn_,
      std::string Exceptions_,
      uint32_t ret_) {
    fileIn = fileIn_;
    ExceptionsMess=Exceptions_;
    ret = ret_;
  }
  std::string fileIn;
  std::string ExceptionsMess;
  uint32_t ret;
};

class PDFWrapperDoc_GetWrapperType:public ::testing::TestWithParam<GetWrapperType_P> {

};

struct GetCryptographicFilter_P {
  GetCryptographicFilter_P(
      std::string fileIn_,
      std::wstring wsGraphicFilter_,
      float fVersion_,
      bool ret_,
      std::string ExceptionsMess_) {
    fileIn =fileIn_;
    graphic_filter = wsGraphicFilter_;
    version_num =fVersion_;
    ret = ret_;
    ExceptionsMess = ExceptionsMess_;
  }
  std::string fileIn;
  std::wstring graphic_filter;
  float version_num;
  bool ret;
  std::string ExceptionsMess;
};

class PDFWrapperDoc_GetCryptographicFilter:public ::testing::TestWithParam<GetCryptographicFilter_P> {

};

struct GetPayLoadSize_P {
  GetPayLoadSize_P(
      std::string fileIn_,
      uint32_t PayLoadSize_,
      std::string ExceptionMess_) {
    fileIn =fileIn_;
    PayLoadSize = PayLoadSize_;
    ExceptionMess = ExceptionMess_;
  }
  std::string fileIn;
  uint32_t PayLoadSize;
  std::string ExceptionMess;
};

class PDFWrapperDoc_GetPayLoadSize:public ::testing::TestWithParam<GetPayLoadSize_P> {

};

struct GetPayloadFileName_P {
  GetPayloadFileName_P(
      std::string fileIn_,
      std::wstring wsFileName_,
      bool ret_,
      std::string ExceptionMess_) {
    fileIn = fileIn_;
    file_name = wsFileName_;
    ret = ret_;
    ExceptionMess = ExceptionMess_;
  }
  std::string fileIn;
  std::wstring file_name;
  bool ret;
  std::string ExceptionMess;
};

class PDFWrapperDoc_GetPayloadFileName:public ::testing::TestWithParam<GetPayloadFileName_P> {

};

struct CreateUnencryptedWrapper_P {
  CreateUnencryptedWrapper_P(
      std::string fileIn_,
      // std::string fileOut_,
      bool ret_,
      std::string ExceptionMess_) {
    fileIn = fileIn_;
    //fileout = fileOut_;
    ret = ret_;
    ExceptionMess= ExceptionMess_;
  }
  std::string fileIn;
  //std::string fileout;
  bool ret;
  std::string ExceptionMess;
};

class PDFUnencryptedWrapperCreator_CreateUnencryptedWrapper:public::testing::TestWithParam<CreateUnencryptedWrapper_P> {

};

struct SetPayloadInfo_P {
  SetPayloadInfo_P(
      std::wstring wsSubType_,
      std::wstring wsFileName_,
      std::wstring wsDescription_,
      std::string outfile_,
      float fVersion_,
      std::string ExceptionMess_) {
    sub_type = wsSubType_;
    file_name= wsFileName_;
    description = wsDescription_;
    fileout = outfile_;
    version_num = fVersion_;
    ExceptionsMess = ExceptionMess_;
  }
  std::wstring sub_type;
  std::wstring file_name;
  std::wstring description;
  std::string fileout;
  float version_num;
  std::string ExceptionsMess;
};

class PDFUnencryptedWrapperCreator_SetPayloadInfo:public::testing::TestWithParam<SetPayloadInfo_P> {
 public:
  static void SetUpTestCase();
  static void TearDownTestCase();
};

#endif // RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_UNITTEST_PDFOBJECTMODEL_H_
