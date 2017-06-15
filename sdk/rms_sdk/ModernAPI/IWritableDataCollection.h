#ifndef IWRITABLEDATACOLLECTION_H
#define IWRITABLEDATACOLLECTION_H

#include "IDataCollection.h"
#include "ModernAPIExport.h"
#include "IAuthenticationCallback.h"

namespace rmscore{
namespace modernapi{

/**
 * @brief The IWritableDataCollection class is an extension of IDataCollection which can be inserted into, and also serialized
 */
class DLL_PUBLIC_RMS IWritableDataCollection : public IDataCollection{

public:

  /**
   * @brief Gets a new DataCollection which can be inserted into
   * @param AuthenticationCallback - Handles OAuth requests when obtaining signing certificate
   * @param UserEmail - Email of user signing the document
   * @param ClientID - Provided by application, used for authentication
   * @return
   */
  static std::shared_ptr<IWritableDataCollection> AcquireWritableDataCollection(
          std::string& UserEmail,
          IAuthenticationCallback& AuthenticationCallback,
          std::string& ClientID);

  /**
   * @brief Inserts data into the Data Collection
   * @param Key
   * @param Value
   * @throws Exception if DataCollection is read-only
   */
  virtual void AddData(std::string& Key, std::string& Value) = 0;

  /**
   * @brief Signs the data collection, and returns it in JSON format
   * @return
   */
  virtual std::string SignAndSerializeDataCollection() = 0;
};

}
}

#endif // IWRITABLEDATACOLLECTION_H
