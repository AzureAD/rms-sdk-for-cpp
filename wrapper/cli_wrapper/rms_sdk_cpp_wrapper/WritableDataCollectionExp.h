#pragma once

#include "IAuthenticationCallbackExp.h"
#include "IWritableDataCollection.h"
#include "ModernAPI\WritableDataCollection.h"
#include "ModernAPI\icontext.h"
#include "ModernAPI\IAuthenticationCallback.h"
#include "IAuthenticationCallbackWrapper.h"

namespace rmscore {
	namespace wrapper {

		/// <summary>
		/// Implementation of IWritableDataCollection
		/// 
		/// Wraps a pointer to a std::shared_ptr for an unmanaged WritableDataCollection
		/// </summary>
		public ref class WritableDataCollectionExp : public IWritableDataCollection
		{
		public:

			WritableDataCollectionExp(System::String^ userEmail, IAuthenticationCallbackExp^ authenticationCallback, System::String^ clientId);
			
			// Inherited via IWritableDataCollection
			virtual bool ContainsKey(System::String ^key);
			virtual System::String ^ GetDataValue(System::String ^ key);
			virtual bool IsVerified();
			virtual void AddDataValue(System::String ^ key, System::String ^ value);
			virtual System::String ^ SignAndSerializeData();
			static void InitalizeContext();

		private:
			WritableDataCollection* mWritableDataCollectionPtr;
		};
	}
}



