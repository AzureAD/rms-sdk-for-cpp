#pragma once
#include "rms_sdk\ModernAPI\AuthenticationParameters.h"
#include "rms_sdk\ModernAPI\IAuthenticationCallback.h"
#include "rms_sdk\ModernAPI\IConsentCallback.h"
#include "IAuthenticationCallbackEx.h"
#include <vcclr.h>

using namespace rmscore::modernapi;
using namespace sdkwrapper;

class AuthenticationCallbackWrapper : public IAuthenticationCallback
{
public:
	AuthenticationCallbackWrapper(gcroot<IAuthenticationCallbackEx^> callback);
	virtual std::string GetToken(std::shared_ptr<AuthenticationParameters>& authenticationParameters);
private:
	gcroot<IAuthenticationCallbackEx^> authenticationCallbackEx;
};

class DummyConsentCallback : public IConsentCallback
{
public:
	ConsentList Consents(ConsentList& consents) override { return ConsentList(); };
};