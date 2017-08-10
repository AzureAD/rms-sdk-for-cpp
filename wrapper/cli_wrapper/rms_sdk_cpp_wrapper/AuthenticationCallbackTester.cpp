#include "stdafx.h"
#include "AuthenticationCallbackTester.h"

AuthenticationCallbackTester::AuthenticationCallbackTester() {

}

System::String^ AuthenticationCallbackTester::TestRoute(System::String^ resource, System::String^ authority, System::String^ redirect, System::String^ userID, IAuthenticationCallbackExp^ authCallbackEx) {
	IAuthenticationCallbackWrapper callbackWrapper(authCallbackEx);
	std::string unmanagedResource = msclr::interop::marshal_as<std::string>(resource);
	std::string unmanagedAuthority = msclr::interop::marshal_as<std::string>(authority);
	std::string unmanagedScope = msclr::interop::marshal_as<std::string>(redirect);
	std::string unmanagedUserID = msclr::interop::marshal_as<std::string>(userID);
	auto spAuthParams = std::make_shared<AuthenticationParameters>(unmanagedAuthority, unmanagedResource, unmanagedScope, unmanagedUserID);
	std::string result = callbackWrapper.GetToken(spAuthParams);
	return gcnew System::String(result.c_str());
}
