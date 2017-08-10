#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H

#include "AuthenticationCallbackImpl.h"
#include "IAuthenticationCallback.h"
#include "IAuthenticationCallbackImpl.h"
#include "ModernAPIExport.h"
#include "Core/IStringSigner.h"
#include "Platform/Json/IJsonObject.h"
#include "Platform/Json/IJsonParser.h"
#include "ModernAPI/RMSExceptions.h"
#include "Common/tools.h"
#include "Common/Constants.h"
#include "Core/IStringVerifier.h"
#include <vector>
#include <memory>

using namespace rmscore::platform::json;
using namespace rmscore::common;
using namespace rmscore::core;
using namespace std;

namespace rmscore{
namespace modernapi{

class DataCollection;

class DLL_PUBLIC_RMS DataCollection
{
public:

  /**
   * @brief Parses out a Data Collection from JSON, and returns a read-only copy of the data
   * @param dataCollectionJson - JSON data
   * @return
   */
  static DataCollection* AcquireVerifiedDataCollection(std::string& dataCollectionJson);

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
  std::string GetDataValue(std::string& Key);

  /**
   * @brief Returns all key/value pairs in the data collection
   * @return
   */
  std::vector<std::pair<std::string&, std::string&>> GetKeyValuePairs();

  /**
   * @brief Checks if this data collection is verified (and thus read-only)
   * @return
   */
  bool IsVerified();

  /**
   * @brief Get the signature associated with this DataCollection
   * @return
   */
  Signature GetSignature();

protected:
  bool verified;
  Signature signature;
  std::map<std::string, std::string> collectedData;
  std::string GetDataHash();
  std::shared_ptr<IJsonObject> ConvertDataToJson();
  DataCollection();

private:
  DataCollection(std::string& json);
  bool ValidateSelf();

};

}
}


#endif // DATACOLLECTION_H
