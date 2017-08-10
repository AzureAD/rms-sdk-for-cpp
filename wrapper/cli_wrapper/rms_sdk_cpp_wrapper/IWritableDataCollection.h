#pragma once
#include "IDataCollection.h"

namespace rmscore {
	namespace wrapper {

		/// <summary>
		/// Extension of IDataCollection to allow addition and serialization
		/// 
		/// Used to keep the relationship between DataCollection and WritableDataCollection
		/// </summary>
		public interface class IWritableDataCollection : public IDataCollection {

			/// <summary>
			/// Inserts a key/value pair into the data collection
			/// </summary>
			/// <param name="key"></param>
			/// <param name="value"></param>
			virtual void AddDataValue(System::String^ key, System::String^ value) = 0;

			/// <summary>
			/// Serializes the data collection, and converts it into a string
			/// </summary>
			/// <returns></returns>
			virtual System::String^ SignAndSerializeData() = 0;
		};
	}
}