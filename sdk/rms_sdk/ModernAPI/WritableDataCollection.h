#ifndef WRITABLEDATACOLLECTION_H
#define WRITABLEDATACOLLECTION_H

#include "DataCollection.h"
#include "ModernAPIExport.h"
#include "IAuthenticationCallback.h"
#include "IWritableDataCollection.h"

namespace rmscore{
namespace modernapi{

/**
 * @brief The WritableDataCollection class implements IWritableDataCollection
 */
class WritableDataCollection : public IWritableDataCollection{

friend class IWritableDataCollection;

public:

  /**
   * @brief Inserts data into the Data Collection
   * @param Key
   * @param Value
   * @throws Exception if DataCollection is read-only
   */
  virtual void AddData(std::string& Key, std::string& Value) override;

  /**
    * @brief Checks if the data collection has been signedAndSerialized yet. If so, return true. If not, return false;
    * @return
    */
  virtual bool IsVerified() override;

  /**
   * @brief Signs the data collection, and returns it in JSON format
   * @return
   */
  virtual std::string SignAndSerializeDataCollection() override;

private:
    WritableDataCollection(std::string &UserEmail, IAuthenticationCallback &AuthenticationCallback, std::string &ClientID);
};

}
}

#endif // WRITABLEDATACOLLECTION_H
