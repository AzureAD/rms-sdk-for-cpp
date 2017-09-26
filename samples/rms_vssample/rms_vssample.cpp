// rms_vssample.cpp : Defines the entry point for the console application.

#include <sstream>
#include <iostream>
#include <fstream>

#include "rmssdk\rms.h"

using namespace rmscore::modernapi;
using namespace rmscore::core;
using namespace std;

const string USERID = "Your-User-Id";
const string CLIENTID = "Your-Registered-Client-ID";
const string REDIRECTURI = "Your-Registered-Redirection-URI://authorize";

class Auth : public IAuthenticationCallback
{
private:
  std::string clientId_;
  std::string redirectUrl_;

public:
  Auth(const std::string& clientID, const std::string& redirectURL);
  ~Auth();
  virtual std::string GetToken(shared_ptr<AuthenticationParameters>& ap) override;
};

std::string Auth::GetToken(std::shared_ptr< AuthenticationParameters > &ap)
{
  return "Plug In Access Token";
}

Auth::Auth(const std::string& clienID, const std::string& redirectURL)
{
  clientId_ = clienID;
  redirectUrl_ = redirectURL;
}

Auth::~Auth() {};

vector<UserRights> GetUserRights()
{
  vector<UserRights> userRights;
  rmscore::modernapi::UserList  users;
  rmscore::modernapi::RightList rights;
  std::string username = USERID;
  users.push_back(username);

  rights.push_back(rmscore::modernapi::CommonRights::Owner());
  rights.push_back(rmscore::modernapi::CommonRights::View());
  rights.push_back(rmscore::modernapi::EditableDocumentRights::Edit());

  if ((users.size() > 0) && (rights.size() > 0)) {
    userRights.push_back(UserRights(users, rights));
  }

  return userRights;
}

shared_ptr<rmscore::modernapi::UserPolicy> CreateDefaultPolicy(vector<UserRights> &rights)
{
  const std::string clientID = CLIENTID;
  const std::string redirectURL = REDIRECTURI;
  Auth auth(clientID, redirectURL);
  auto endValidation = chrono::system_clock::now() + chrono::hours(48);
  PolicyDescriptor desc(rights);

  desc.Referrer(make_shared<string>("https://client.test.app"));
  desc.ContentValidUntil(endValidation);
  desc.AllowOfflineAccess(false);
  desc.Name("Test Name");
  desc.Description("Test Description");
  std::shared_ptr<std::atomic<bool>>cancelState = false;

  return UserPolicy::Create(desc, USERID, auth, USER_None, cancelState);
}

void EncryptAndDecryptStream(const shared_ptr<rmscore::modernapi::UserPolicy>  & policy)
{
  std::stringstream Teststream;
  Teststream.str("some text for the stream");
  cout << "input stream: " << Teststream.str() << endl;
  std::filebuf fbo;
  fbo.open("protected.txt", std::ios::out | std::ios::binary);
  auto ostream = make_shared<std::ostream>(&fbo);
  auto outSharedStream = rmscrypto::api::CreateStreamFromStdStream(ostream);

  auto pStream = CustomProtectedStream::Create(policy, outSharedStream, 0, 0);
  Teststream.seekg(0, ios::end);
  stringstream::pos_type size = Teststream.tellg();
  int encSize = CustomProtectedStream::GetEncryptedContentLength(policy, size);

  int b = pStream->Write((uint8_t*)Teststream.str().c_str(), size);
  bool s = pStream->Flush();

  //Now Decrypt the encrypted stream
std:filebuf fbi;
  fbi.open("protected.txt", std::ios::in | std::ios::binary);
  auto istream = make_shared<std::istream>(&fbi);
  auto encryptedSharedStream = rmscrypto::api::CreateStreamFromStdStream(istream);

  auto poStream = CustomProtectedStream::Create(policy, encryptedSharedStream, 0, encryptedSharedStream->Size());
  char outbuff[2048] = { 0 };
  poStream->Read((uint8_t *)outbuff, sizeof(outbuff));

  std::stringstream Teststreamout;
  Teststreamout << outbuff;
  cout << "decrypted stream: " << Teststreamout.str() << endl;
  return;
}

shared_ptr<rmscore::modernapi::UserPolicy> CreateUserPolicyFromTemplate()
{
  const std::string clientID = CLIENTID;
  const std::string redirectURL = REDIRECTURI;
  Auth auth(clientID, redirectURL);
  std::shared_ptr<std::atomic<bool>>cancelState = false;

  auto templatesFuture = TemplateDescriptor::GetTemplateListAsync(USERID, auth, launch::deferred, cancelState);
  auto templates = templatesFuture.get();
  rmscore::modernapi::AppDataHashMap signedData;

  auto policy = UserPolicy::CreateFromTemplateDescriptor((*templates)[0], USERID, auth, USER_None, signedData, cancelState);

  return policy;
}

int main(int argc, char* argv[])
{
  vector<string> userlist, rolelist;
  auto sroles = new UserRoles(userlist, rolelist);
  auto reviewer = rmscore::modernapi::Roles::Reviewer();
  auto owner = CommonRights::AuditedExtract;
  auto env = rmscore::modernapi::RMSEnvironment();
  //Change this to rmscore::modernapi::IRMSEnvironment::LoggerOption::Never to disable logging
  env->LogOption(rmscore::modernapi::IRMSEnvironment::LoggerOption::Always);

  //Try with adhoc rights
  vector<UserRights> userRights = GetUserRights();
  auto policyRights = CreateDefaultPolicy(userRights);
  EncryptAndDecryptStream(policyRights);

  //Try with default dummy template.
  auto policyTemplates = CreateUserPolicyFromTemplate();
  EncryptAndDecryptStream(policyTemplates);

  return 0;
}
