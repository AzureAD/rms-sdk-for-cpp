#include "stdafx.h"
#include <msclr\marshal_cppstd.h>
#include "IAuthenticationCallbackWrapper.h"

IAuthenticationCallbackWrapper::IAuthenticationCallbackWrapper(gcroot<IAuthenticationCallbackExp^> callback): authenticationCallbackExp(callback)
{
}

std::string IAuthenticationCallbackWrapper::GetToken(std::shared_ptr<AuthenticationParameters>& authenticationParameters)
{
	System::String^ managedAuthority = gcnew System::String(authenticationParameters->Authority().c_str());
	System::String^ managedResource = gcnew System::String(authenticationParameters->Resource().c_str());
	System::String^ managedScope = gcnew System::String(authenticationParameters->Scope().c_str());
	System::String^ managedUserID = gcnew System::String(authenticationParameters->UserId().c_str());
	System::String^ managedResult = authenticationCallbackExp->GetAccessToken(managedAuthority, managedResource, managedScope, managedUserID);
	return msclr::interop::marshal_as<std::string>(managedResult);
}
