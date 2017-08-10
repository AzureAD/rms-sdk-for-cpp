#pragma once

#include "IAuthenticationCallbackExp.h"

namespace rmscore {
	namespace wrapper {

		/// <summary>
		/// Interface for a DataCollection
		/// 
		/// Used to maintain the parent/child relationship between DataCollection and WritableDataCollection
		/// </summary>
		public interface class IDataCollection {

			/// <summary>
			/// Checks if the data collection contains the specified key
			/// </summary>
			/// <param name="Key"></param>
			/// <returns></returns>
			virtual bool ContainsKey(System::String^ key) = 0;

			/// <summary>
			/// Gets the key for the specified data value
			/// </summary>
			/// <param name="Key"></param>
			/// <returns></returns>
			virtual System::String^ GetDataValue(System::String^ key) = 0;

			/// <summary>
			/// Checks if the Data Collection is verified
			/// </summary>
			/// <returns></returns>
			virtual bool IsVerified() = 0;
		};

	}
}