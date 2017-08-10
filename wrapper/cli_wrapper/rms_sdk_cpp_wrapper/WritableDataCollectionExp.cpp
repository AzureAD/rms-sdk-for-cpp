#include "stdafx.h"
#include "WritableDataCollectionExp.h"
#include "ModernAPI\IAuthenticationCallback.h"
#include <msclr\marshal_cppstd.h>

namespace rmscore {
	namespace wrapper {

		WritableDataCollectionExp::WritableDataCollectionExp(System::String ^ userEmail, IAuthenticationCallbackExp ^ authenticationCallback, System::String ^ clientId)
		{
			std::string unmanagedUserEmail = msclr::interop::marshal_as<std::string>(userEmail);
			std::string unmanagedClientId = msclr::interop::marshal_as<std::string>(clientId);
			IAuthenticationCallbackWrapper callbackWrapper(authenticationCallback);
			mWritableDataCollectionPtr = WritableDataCollection::AcquireWritableDataCollection(unmanagedUserEmail, callbackWrapper, unmanagedClientId);
		}

		bool WritableDataCollectionExp::ContainsKey(System::String ^ key)
		{
			std::string unmanagedKey = msclr::interop::marshal_as<std::string>(key);
			return mWritableDataCollectionPtr->ContainsKey(unmanagedKey);
		}

		System::String ^ WritableDataCollectionExp::GetDataValue(System::String ^ key)
		{
			std::string unmanagedKey = msclr::interop::marshal_as<std::string>(key);
			std::string unmanagedResult = mWritableDataCollectionPtr->GetDataValue(unmanagedKey);
			return gcnew System::String(unmanagedResult.c_str());
		}

		bool WritableDataCollectionExp::IsVerified()
		{
			return mWritableDataCollectionPtr->IsVerified();
		}

		void WritableDataCollectionExp::AddDataValue(System::String ^ key, System::String ^ value)
		{
			std::string unmanagedKey = msclr::interop::marshal_as<std::string>(key);
			std::string unmanagedValue = msclr::interop::marshal_as<std::string>(value);
			mWritableDataCollectionPtr->AddData(unmanagedKey, unmanagedValue);
		}

		System::String ^ WritableDataCollectionExp::SignAndSerializeData()
		{
			std::string unmanagedJson = mWritableDataCollectionPtr->SignAndSerializeDataCollection();
			return gcnew System::String(unmanagedJson.c_str());
		}
		void WritableDataCollectionExp::InitalizeContext()
		{
			rmscore::modernapi::IContext* context = rmscore::modernapi::IContext::MakeContext(std::string("dummy_args"));
		}
	}
}