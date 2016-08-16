#ifndef DUMMYAUTHCALLBACK_H
#define DUMMYAUTHCALLBACK_H
#include "../../ModernAPI/IAuthenticationCallbackImpl.h"
#include <iostream>
#include <memory>

using namespace rmscore;
using namespace rmscore::modernapi;

namespace UnitTests {
namespace RestClientsUT {

class DummyAuthCallback : public IAuthenticationCallbackImpl
{
public:
    DummyAuthCallback();
    bool NeedsChallenge() const override;
    std::string GetAccessToken(const AuthenticationChallenge &challenge) override;
};

} //UnitTests
} //RestClientsUT

#endif // DUMMYAUTHCALLBACK_H
