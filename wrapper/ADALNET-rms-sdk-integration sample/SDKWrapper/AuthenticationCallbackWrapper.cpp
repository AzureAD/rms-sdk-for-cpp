#include "stdafx.h"
#include <msclr\marshal_cppstd.h>
#include "AuthenticationCallbackWrapper.h"


AuthenticationCallbackWrapper::AuthenticationCallbackWrapper(gcroot<IAuthenticationCallbackEx^> callback) : authenticationCallbackEx(callback)
{
}

std::string AuthenticationCallbackWrapper::GetToken(std::shared_ptr<AuthenticationParameters>& authenticationParameters)
{
	System::String^ managedAuthority = gcnew System::String(authenticationParameters->Authority().c_str());
	System::String^ managedResource = gcnew System::String(authenticationParameters->Resource().c_str());
	System::String^ managedScope = gcnew System::String(authenticationParameters->Scope().c_str());
	System::String^ managedUserID = gcnew System::String(authenticationParameters->UserId().c_str());
	System::String^ managedResult = authenticationCallbackEx->GetAccessToken(managedAuthority, managedResource, managedScope, managedUserID);
	return msclr::interop::marshal_as<std::string>(managedResult);
}
