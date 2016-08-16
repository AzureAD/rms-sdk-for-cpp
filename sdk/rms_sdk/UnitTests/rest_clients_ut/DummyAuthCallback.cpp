#include "DummyAuthCallback.h"
#include <QtGlobal>

namespace UnitTests {
namespace RestClientsUT {

DummyAuthCallback::DummyAuthCallback() : IAuthenticationCallbackImpl()
{

}
bool DummyAuthCallback::NeedsChallenge() const
{
    return false;
}
std::string DummyAuthCallback::GetAccessToken(const AuthenticationChallenge &challenge)
{
    Q_UNUSED(challenge);
    return "";
}
} //UnitTests
} //RestClientsUT
