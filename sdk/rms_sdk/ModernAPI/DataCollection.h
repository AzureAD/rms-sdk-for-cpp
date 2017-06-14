#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H

#include "IAuthenticationCallback.h"
#include "ModernAPIExport.h"

namespace rmscore{
namespace modernapi{

class DataCollection
{
public:

    /**
   * @brief Gets a new DataCollection which can be inserted into
   * @param AuthenticationCallback - Handles OAuth requests when obtaining signing certificate
   * @param UserEmail - Email of user signing the document
   * @param ClientID - Provided by application, used for authentication
   * @return
   */
  static std::shared_ptr<DataCollection> AcquireDataCollection(
          IAuthenticationCallback& AuthenticationCallback,
          std::string& UserEmail,
          std::string& ClientID);

  /**
   * @brief Parses out a Data Collection from JSON, and returns a read-only copy of the data
   * @param dataCollectionJson - JSON data
   * @return
   */
  static std::shared_ptr<DataCollection> AcquireVerifiedDataCollection(std::string& dataCollectionJson);

  /**
   * @brief Inserts data into the Data Collection
   * @param Key
   * @param Value
   * @throws Exception if DataCollection is read-only
   */
  void AddContent(std::string& Key, std::string& Value);

  /**
   * @brief Checks if the key exists in the data collection
   * @param Key
   * @return
   */
  bool ContainsKey(std::string& Key);

  /**
   * @brief Grabs the data associated with a key
   * @param Key
   * @return
   */
  std::string GetDataValue(std::string Key);

  /**
   * @brief Returns every key in the data collection
   * @return
   */
  std::vector<std::string> GetKeySet();

  /**
   * @brief Returns every value in the data collection
   * @return
   */
  std::vector<std::string> GetValueSet();

  /**
   * @brief Checks if this data collection is verified (and thus read-only)
   * @return
   */
  bool IsVerified();

private:

  DataCollection(IAuthenticationCallback& AuthenticationCallback, std::string& UserEmail, std::string& ClientID);
  DataCollection(std::string& json);
  bool verified;

};

}
}


#endif // DATACOLLECTION_H
