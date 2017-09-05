#pragma once
#include "AuthenticationCallbackWrapper.h"
#include "IAuthenticationCallbackEx.h"

namespace sdkwrapper {
	public ref class PFileConverterEx
	{
	public:
		PFileConverterEx(IAuthenticationCallbackEx^ callback);
		~PFileConverterEx() { }
		System::Boolean ConvertFromPFile(System::String^ filename, System::String^ userEmail, System::String^ redirectUrl);
	private:
		AuthenticationCallbackWrapper* authCallback;
	};
}