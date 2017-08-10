#include "stdafx.h"
#include <msclr\marshal_cppstd.h>
#include "DataCollectionExp.h"


namespace rmscore {
	namespace wrapper {
		DataCollectionExp::DataCollectionExp(String^ dataJson) {
			std::string unmanagedDataJson = msclr::interop::marshal_as<std::string>(dataJson);
			mDataCollectionPointer = DataCollection::AcquireVerifiedDataCollection(unmanagedDataJson);
		}

		bool DataCollectionExp::ContainsKey(System::String ^ key)
		{
			std::string unmanagedKey = msclr::interop::marshal_as<std::string>(key);
			return mDataCollectionPointer->ContainsKey(unmanagedKey);
		}

		System::String ^ DataCollectionExp::GetDataValue(System::String ^ key)
		{
			std::string unmanagedKey = msclr::interop::marshal_as<std::string>(key);
			std::string result = mDataCollectionPointer->GetDataValue(unmanagedKey);
			return gcnew String(result.c_str());
		}

		bool DataCollectionExp::IsVerified()
		{
			return mDataCollectionPointer->IsVerified();
		}

		System::String^ DataCollectionExp::GetCertificateJson()
		{
			auto certificateVector = mDataCollectionPointer->GetSignature().Certificate->Stringify();
			std::string certificateString(certificateVector.begin(), certificateVector.end());
			return gcnew String(certificateString.c_str());
		}

		
	}
}