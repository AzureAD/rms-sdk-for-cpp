#ifndef IDATACOLLECTION_H
#define IDATACOLLECTION_H

#include "ModernAPIExport.h"
#include "IAuthenticationCallback.h"
#include <vector>

namespace rmscore {
namespace modernapi{

class DLL_PUBLIC_RMS IDataCollection
{
public:

  /**
   * @brief Parses out a Data Collection from JSON, and returns a read-only copy of the data
   * @param dataCollectionJson - JSON data
   * @return
   */
  static std::shared_ptr<IDataCollection> AcquireVerifiedDataCollection(std::string& dataCollectionJson);

  /**
   * @brief Checks if the key exists in the data collection
   * @param Key
   * @return
   */
  virtual bool ContainsKey(std::string& Key){
      return true;
  };

  /**
    * @brief Grabs the data associated with a key
    * @param Key
    * @return
    */
  virtual std::string GetDataValue(std::string Key){
      return "";
    };

  /**
   * @brief Returns all key/value pairs in the data collection
   * @return
   */
  virtual std::vector<std::pair<std::string, std::string>> GetKeyValuePairs(){
      return std::vector<std::pair<std::string, std::string>>();
  }

  /**
   * @brief Checks if this data collection is verified (and thus read-only)
   * @return
   */
  virtual bool IsVerified() = 0;

};

}
}

#endif // IDATACOLLECTION_H
