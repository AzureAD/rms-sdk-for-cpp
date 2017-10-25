#ifndef RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_UNITTEST_PROTECTOR_H_
#define RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_UNITTEST_PROTECTOR_H_

#include "depend.h"
#include <string>

//****************WithWrapperCreate Test***************************************************

struct Create_P{
    Create_P(std::string fileIn_,
             std::string outputFileName_,
             std::string Exceptions_,
             bool ret_)
    {
        fileIn = fileIn_;
        outputFileName = outputFileName_;
        ExceptionsMess=Exceptions_;
        ret = ret_;
    }
    std::string fileIn;
    std::string outputFileName;
    std::string ExceptionsMess;
    bool ret;
};
class ProtectorWithWrapper_Create : public ::testing::TestWithParam<Create_P>
{

};

//****************IsProtected Test***************************************************
struct IsProtected_P{
    IsProtected_P(std::string fileIn_,
                  std::string ExceptionsMess_,
    bool ret_)
    {
        fileIn = fileIn_;
        ret = ret_;
        ExceptionsMess=ExceptionsMess_;
    }
    std::string fileIn;
    std::string ExceptionsMess;
    bool ret;
};
class ProtectorWithWrapper_IsProtected:public ::testing::TestWithParam<IsProtected_P>
{

};

//********************protector***********************************************
struct Unprotector_P{
    Unprotector_P(std::string fileIn_,
                  std::string fileout_,
                  std::string ExceptionsMess_,
                  bool cancelState_)
    {
        fileIn = fileIn_;
        fileout =fileout_;
        ExceptionsMess=ExceptionsMess_;
        cancelState = cancelState_;


    }
    std::string fileIn;
    std::string fileout;
    std::string ExceptionsMess;
    bool cancelState;
};
class ProtectorWithWrapper_Unprotector:public ::testing::TestWithParam<Unprotector_P>
{

};

//************ProtectWithTemplate********************************************
struct ProtectWithTemplate_P{
    ProtectWithTemplate_P(std::string fileIn_,
                           std::string fileout_,
                           std::string ExceptionsMess_,
                           bool cancelState_,
                           bool ret_)
             {
                 fileIn = fileIn_;
                 fileout =fileout_;
                 ExceptionsMess=ExceptionsMess_;
                 cancelState = cancelState_;
                 ret =ret_;
             }
    std::string fileIn;
    std::string fileout;
    std::string ExceptionsMess;
    bool cancelState;
    bool ret;
};
class ProtectorWithWrapper_ProtectWithTemplate:public ::testing::TestWithParam<ProtectWithTemplate_P>
{

};

//************ProtectWithTemplate********************************************
struct ProtectWithCustomRights_P{
    ProtectWithCustomRights_P(std::string fileIn_,
                           std::string fileout_,
                           //std::string CLIENTEMAIL_,
                           std::string UserLists_,
                           std::string RightList_,
                           std::string ExceptionsMess_,

                           bool cancelState_,
                           bool ret_)
             {
                 fileIn = fileIn_;
                 fileout =fileout_;
                // CLIENTEMAIL=CLIENTEMAIL_;
                 ExceptionsMess=ExceptionsMess_;
                 UserLists = UserLists_;
                 RightList = RightList_;
                 cancelState = cancelState_;
                 ret =ret_;
             }
    std::string fileIn;
    std::string fileout;
    //std::string CLIENTEMAIL;
    std::string UserLists;
    std::string RightList;
    std::string ExceptionsMess;
    bool cancelState;
    bool ret;
};
class ProtectorWithWrapper_ProtectWithCustomRights:public ::testing::TestWithParam<ProtectWithCustomRights_P>
{

};

struct SetWrapper_P{
    SetWrapper_P(std::string wrapperin_,
                 std::string outfile_,
                 std::string ExceptionsMess_,
                 bool ret_)
            {
                wrapperin = wrapperin_;
                outfile = outfile_;
                ExceptionsMess= ExceptionsMess_;
                ret =ret_;
            }
    std::string wrapperin;
    std::string outfile;
    std::string ExceptionsMess;
    bool ret;
};

class ProtectorWithWrapper_SetWrapper:public ::testing::TestWithParam<SetWrapper_P>
{

};

#endif // RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_UNITTEST_PROTECTOR_H_
