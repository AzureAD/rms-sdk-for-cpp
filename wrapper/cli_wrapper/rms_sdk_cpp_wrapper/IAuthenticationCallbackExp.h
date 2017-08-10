#pragma once
public interface class IAuthenticationCallbackExp
{
public:
	virtual System::String^ GetAccessToken(System::String^ authority, System::String^ resource, System::String^ scope, System::String^ userID) = 0;
};

