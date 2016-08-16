#include "OfflinePublishTest.h"
#include "../../RestClients/PublishClient.h"
#include "../../ModernAPI/AuthenticationCallbackImpl.h"
#include "DummyAuthCallback.h"

using namespace std;
using namespace rmscore;
using namespace rmscore::restclients;

namespace UnitTests {
namespace RestClientsUT {
void OfflinePublishTest::testCreatePLWithTemplate()
{
    auto pPublishClient = IPublishClient::Create();
    PublishUsingTemplateRequest req;
    req.bAllowAuditedExtraction = false;
    req.bPreferDeprecatedAlgorithms = false;
    req.templateId = 1234;
    auto d = DummyAuthCallback();
    auto result = pPublishClient->LocalPublishUsingTemplate(req, d, "john.smith@microsoft.com", nullptr, OfflinePublishTest::generateMockCLC);

}

void OfflinePublishTest::testCreatePLCustom()
{

}

std::string OfflinePublishTest::generateMockCLC(std::string email, std::string& outCLC)
{
    return "";
}

} //UnitTests
} //RestClientsUT

