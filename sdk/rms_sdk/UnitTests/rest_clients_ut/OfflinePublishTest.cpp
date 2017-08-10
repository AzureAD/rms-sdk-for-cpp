#include "OfflinePublishTest.h"
#include "../../ModernAPI/AuthenticationCallbackImpl.h"
#include "../../Common/tools.h"
#include "../../ModernAPI/RMSExceptions.h"

#define check(x)     bytes = EncryptBytesToBase64Wrapper(*pPublishClient, vector<uint8_t>(x, 4), key, CipherMode::CIPHER_MODE_CBC4K); \
                     bytes = rsa->PrivateDecrypt(common::ConvertBase64ToBytes(bytes)); \
                     Q_ASSERT(val_checker(bytes));

using namespace std;
using namespace rmscore;
using namespace rmscore::restclients;
using namespace unittest::restclientsut;
using namespace rmscrypto::api;


void OfflinePublishTest::testCreatePLWithTemplate()
{
    auto pPublishClient = dynamic_pointer_cast<PublishClient>(std::make_shared<PublishClient>());
    PublishUsingTemplateRequest req;
    req.bAllowAuditedExtraction = false;
    req.bPreferDeprecatedAlgorithms = false;
    req.templateId = "00000";
    auto em = "john.smith@microsoft.com";
    auto d = DummyAuthCallback();
    auto result = pPublishClient->LocalPublishUsingTemplate(req, d, em, nullptr, OfflinePublishTest::generateMockCLC);
    Q_ASSERT(result.owner == em);
}

void OfflinePublishTest::testCreatePLCustom()
{
    auto pPublishClient = IPublishClient::Create();
    PublishCustomRequest req(false, false);
    req.bAllowAuditedExtraction = false;
    req.bPreferDeprecatedAlgorithms = false;
    req.name = "test name";
    UserRightsRequest r;
    r.users = { "john.smith@microsoft.com" };
    r.rights = { "OWNER" };
    req.userRightsList = { r };
    auto em = "john.smith@microsoft.com";
    auto d = DummyAuthCallback();
    auto result = pPublishClient->LocalPublishCustom(req, d, em, nullptr, OfflinePublishTest::generateMockCLC);
   // Q_ASSERT(result.name == req.name);
}

std::string OfflinePublishTest::generateMockCLC(const std::string email, std::string& outCLC)
{
    outCLC = R"(\"pub\":{\"pld\":\"{\\\"issto\\\":{\\\"em\\\":\\\")" + email + R"(\\\",\\\"id\\\":{\\\"typ\\\":\\\"Federation\\\",\\\"val\\\":\\\"{10f2270c-6f5a-4c92-ab2e-b429731ce7fc}\\\"},\\\"pubk\\\":{\\\"kty\\\":\\\"RSA\\\",\\\"e\\\":\\\"65537\\\",\\\"n\\\":\\\"Kb+ABF60XvYvQ3cL3dFY1ofeIpJhCq7sRyvapgQsLkmwG8vL8HV0RQCd+XxnpXh8dSNXVJchLXGhAHhu0IQZE6rZaLHxj8HUjf9qdrLSiodDMf2WwyaIA5FqFjg6MWv5H8xykSYvTpt3iBc2aJ1gJxgR4mc7EXxES3G2AIkarb4=\\\"}},\\\"iss\\\":{\\\"eurl\\\":\\\"https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/licensing\\\",\\\"iurl\\\":\\\"https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/licensing\\\",\\\"fname\\\":\\\"CorpRights\\\",\\\"id\\\":\\\"{aaa331f3-e659-4ea9-876b-e8b25c7b4996}\\\",\\\"pubk\\\":{\\\"kty\\\":\\\"RSA\\\",\\\"e\\\":\\\"65537\\\",\\\"n\\\":\\\"oxyxbUguymFHLhu80EH9m62c2XLFXbjm87KlEbSFQkieurr8RZgvtEMWdg6jMAkLJ7xPnWs3hEjOHw99irwZ1g5Z+6yYgktWlKibmPs5Lpamd6v53R40fKES/HNWdjbc68xN9tyfts4pOfyJxT5Xgo0rHVset+rDgShE9B3JQbY=\\\"}}}\",\"sig\":{\"alg\":\"SHA1\",\"dig\":\"VQCM58TaSUBRrJdEW9wtPTtiZGEGS3NBq0qHfev6XPDpH7o6hUo1foS1cbUkj+xaG+lt+e1Fx0I4vakopU8PlCy4R3OQztRgn3d6XTYDk7YOxftqj5Uyh0pN3vxXU+XFSpSte4N7wgoseq8r7cCUM7X8BCvMiroKbKDlPOIi7os=\"}})";
    auto returnString = R"({"pld":"{\"hdr\":{\"ver\":\"1.0.0.0\"},\"pub\":{\"pld\":\"{\\\"issto\\\":{\\\"em\\\":\\\")" + email + R"(\\\",\\\"id\\\":{\\\"typ\\\":\\\"Federation\\\",\\\"val\\\":\\\"{10f2270c-6f5a-4c92-ab2e-b429731ce7fc}\\\"},\\\"pubk\\\":{\\\"kty\\\":\\\"RSA\\\",\\\"e\\\":\\\"65537\\\",\\\"n\\\":\\\"Kb+ABF60XvYvQ3cL3dFY1ofeIpJhCq7sRyvapgQsLkmwG8vL8HV0RQCd+XxnpXh8dSNXVJchLXGhAHhu0IQZE6rZaLHxj8HUjf9qdrLSiodDMf2WwyaIA5FqFjg6MWv5H8xykSYvTpt3iBc2aJ1gJxgR4mc7EXxES3G2AIkarb4=\\\"}},\\\"iss\\\":{\\\"eurl\\\":\\\"https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/licensing\\\",\\\"iurl\\\":\\\"https://48fc04bd-c84b-44ac-91b7-a4c5eefd5ac1.rms.na.aadrm.com/_wmcs/licensing\\\",\\\"fname\\\":\\\"CorpRights\\\",\\\"id\\\":\\\"{aaa331f3-e659-4ea9-876b-e8b25c7b4996}\\\",\\\"pubk\\\":{\\\"kty\\\":\\\"RSA\\\",\\\"e\\\":\\\"65537\\\",\\\"n\\\":\\\"oxyxbUguymFHLhu80EH9m62c2XLFXbjm87KlEbSFQkieurr8RZgvtEMWdg6jMAkLJ7xPnWs3hEjOHw99irwZ1g5Z+6yYgktWlKibmPs5Lpamd6v53R40fKES/HNWdjbc68xN9tyfts4pOfyJxT5Xgo0rHVset+rDgShE9B3JQbY=\\\"}}}\",\"sig\":{\"alg\":\"SHA1\",\"dig\":\"VQCM58TaSUBRrJdEW9wtPTtiZGEGS3NBq0qHfev6XPDpH7o6hUo1foS1cbUkj+xaG+lt+e1Fx0I4vakopU8PlCy4R3OQztRgn3d6XTYDk7YOxftqj5Uyh0pN3vxXU+XFSpSte4N7wgoseq8r7cCUM7X8BCvMiroKbKDlPOIi7os=\"}},\"pri\":{\"issd\":\"0001-01-01T00:00:00\",\"exp\":\"0001-01-01T00:00:00\",\"prik\":{\"kty\":\"RSA\",\"d\":\"oZ8L+zbGOK2Aktm3y77u0Z3lnR61G7nk6TLXSTNGGOdFG2Pd5+fOgPwz0BOfdAIqoq53HZt8Chuzm7SUpoLsMIkAiVWjpdBfA4I9pRHQRwkJA2IwtQQP3mKv9xN+5neEBqChmtBJ5t83LvzhShmLJuupn/W955SBV+4ICMZ56BU=\"},\"id\":\"{66e5648b-765e-43a2-bd8c-9dca3dc66475}\"}}","sig":{"alg":"SHA1","dig":"k3NIX7oX8Faj3LNjYeJyo1PxJ7w+cHnC6VG/fshtZWnSqCwOV//J/wawA+yRJ2mLE6dGyhfyUV7lnYPR9uhYqP8P8CKYX7XLlRBibkiNbrbnhP5sRDs7wAdmGBVLb25LmEdtbEr+mnxLfeXYihxZPR8KZtcm3QpyAZNxJUDBoUo="}})";
    return returnString;
}


