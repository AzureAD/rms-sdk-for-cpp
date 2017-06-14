#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H

#include "ModernAPIExport.h"
#include <vector>
#include <memory>

namespace rmscore{
namespace modernapi{

class DLL_PUBLIC_RMS DataCollection
{
public:

  /**
   * @brief Parses out a Data Collection from JSON, and returns a read-only copy of the data
   * @param dataCollectionJson - JSON data
   * @return
   */
  static std::shared_ptr<DataCollection> AcquireVerifiedDataCollection(std::string& dataCollectionJson);

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
   * @brief Returns all key/value pairs in the data collection
   * @return
   */
  std::vector<std::pair<std::string, std::string>> GetKeyValuePairs();

  /**
   * @brief Checks if this data collection is verified (and thus read-only)
   * @return
   */
  bool IsVerified();

private:

  DataCollection(std::string& json);
  bool verified;

};

}
}


#endif // DATACOLLECTION_H
