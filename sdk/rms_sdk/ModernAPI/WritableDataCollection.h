#ifndef WRITABLEDATACOLLECTION_H
#define WRITABLEDATACOLLECTION_H

#include "DataCollection.h"
#include "ModernAPIExport.h"
#include "IAuthenticationCallback.h"
#include "IAuthenticationCallbackImpl.h"
#include "AuthenticationCallbackImpl.h"
#include "RestClients/iclcclient.h"
#include "Common/Constants.h"
#include "Core/IStringSigner.h"

using namespace rmscore::common;
using namespace rmscore::restclients;

namespace rmscore{
namespace modernapi{

class DLL_PUBLIC_RMS WritableDataCollection : public DataCollection{

public:

  /**
   * @brief Gets a new DataCollection which can be inserted into
   * @param AuthenticationCallback - Handles OAuth requests when obtaining signing certificate
   * @param UserEmail - Email of user signing the document
   * @param ClientID - Provided by application, used for authentication
   * @return
   */
  static WritableDataCollection* AcquireWritableDataCollection(
          std::string& UserEmail,
          IAuthenticationCallback& AuthenticationCallback,
          std::string& ClientID);

  /**
   * @brief Inserts data into the Data Collection
   * @param Key
   * @param Value
   * @throws Exception if DataCollection is read-only
   */
  void AddData(std::string& Key, std::string& Value);

  /**
   * @brief Signs the data collection, and returns it in JSON format
   * @return
   */
  std::string SignAndSerializeDataCollection();

private:
  std::shared_ptr<IJsonObject> clientLicensorCertificate;
  WritableDataCollection(std::string& UserEmail, IAuthenticationCallback& AuthenticationCallback, std::string& ClientID);
};

}
}

#endif // WRITABLEDATACOLLECTION_H
