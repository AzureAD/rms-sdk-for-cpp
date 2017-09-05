#include "stdafx.h"
#include "PFileConverterEx.h"
#include "rms_sdk\ModernAPI\FastPFileConverter.h"

#include <msclr\marshal_cppstd.h>
#include <fstream>

using namespace msclr::interop;
using std::string;
using std::shared_ptr;
using std::make_shared;
using namespace sdkwrapper;

PFileConverterEx::PFileConverterEx(IAuthenticationCallbackEx^ callback)
{
	authCallback = new AuthenticationCallbackWrapper(callback);
}

System::Boolean PFileConverterEx::ConvertFromPFile(System::String ^ filename, System::String ^ userEmail, System::String ^ redirectUrl)
{
	string sFilename = marshal_as<string>(filename);
	string sEmail = marshal_as<string>(userEmail);
	string sRedirectUrl = marshal_as<string>(redirectUrl);
	string fileOut;

	// generate output filename
	auto pos = sFilename.find_last_of('.');

	if (pos != string::npos) {
		string ext = sFilename.substr(pos);
		if (ext.compare(".pfile") == 0) {
			fileOut = sFilename.substr(0, pos);
		}
		else {
			auto ppos = ext.find_first_of('p');
			if (ppos != string::npos) {
				fileOut = sFilename.substr(0, pos) + "." + ext.substr(ppos + 1);
			}
			else {
				fileOut = sFilename;
			}
		}
	}

	auto f = new FastPFileConverter();
	auto dummyConsent = new DummyConsentCallback();
	System::Boolean ret = f->ConvertFromPFile(sEmail, sFilename, fileOut, *authCallback, *dummyConsent);
	return ret;
}
