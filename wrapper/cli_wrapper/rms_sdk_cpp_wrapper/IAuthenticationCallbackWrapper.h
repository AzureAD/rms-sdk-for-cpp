#pragma once

#include "IAuthenticationCallbackExp.h"
#include <vcclr.h> 

using namespace rmscore::modernapi;

/// <summary>
/// Implements the AuthenticationCallback wrapper
/// 
/// Takes a IAuthenticationCallbackExp, and passes along the authentication parameters to that object
/// 
/// This allows for the managed IAuthenticationCallbackExp to listen for calls to an unmanaged authentication callback
/// </summary>
class IAuthenticationCallbackWrapper: public IAuthenticationCallback
{
public:
	IAuthenticationCallbackWrapper(gcroot<IAuthenticationCallbackExp^> callback);
	virtual std::string GetToken(std::shared_ptr<AuthenticationParameters>& authenticationParameters);

private:
	gcroot<IAuthenticationCallbackExp^> authenticationCallbackExp;
};

