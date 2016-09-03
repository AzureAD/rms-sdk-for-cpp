#include "DummyAuthCallback.h"
#include <QtGlobal>

namespace unittest {
namespace restclientsut {

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
