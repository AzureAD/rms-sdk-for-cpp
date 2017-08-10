#pragma once

#include "IDataCollection.h"

using namespace rmscore::modernapi;
using namespace System;

namespace rmscore {
	namespace wrapper {
		public ref class DataCollectionExp : public IDataCollection {

		public:

			DataCollectionExp(String^ dataCollectionJson);

			// Inherited via IDataCollection
			virtual bool ContainsKey(System::String ^ key);
			virtual System::String ^ GetDataValue(System::String ^ key);
			virtual bool IsVerified();
			System::String^ GetCertificateJson();

		private:
			DataCollection* mDataCollectionPointer;
		};
	}
}