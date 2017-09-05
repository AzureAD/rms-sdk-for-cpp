#pragma once
namespace sdkwrapper {
	public interface class IAuthenticationCallbackEx
	{
	public:
		virtual System::String^ GetAccessToken(System::String^ authority, System::String^ resource, System::String^ scope, System::String^ userID) = 0;
	};
}