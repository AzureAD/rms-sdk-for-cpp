#ifndef WRITABLEDATACOLLECTION_H
#define WRITABLEDATACOLLECTION_H

#include "DataCollection.h"
#include "ModernAPIExport.h"
#include "IAuthenticationCallback.h"

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
  static std::shared_ptr<WritableDataCollection> AcquireWritableDataCollection(
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
};

}
}

#endif // WRITABLEDATACOLLECTION_H
