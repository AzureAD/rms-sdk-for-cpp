#pragma once

#include "IAuthenticationCallbackExp.h"
#include "ModernAPI\AuthenticationParameters.h"
#include "IAuthenticationCallbackWrapper.h"
#include <msclr\marshal_cppstd.h>

using namespace rmscore::modernapi;

public ref class AuthenticationCallbackTester
{
public:
	AuthenticationCallbackTester();
	System::String^ TestRoute(System::String^ resource, System::String^ authority, System::String^ redirect, System::String^ userID, IAuthenticationCallbackExp^ authCallbackEx);

};

