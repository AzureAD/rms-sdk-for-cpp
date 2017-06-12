#ifndef SEALPOLICY_H
#define SEALPOLICY_H

#include <string>
#include <memory>
#include "Seal.h"
#include "ModernAPIExport.h"
#include "IAuthenticationCallback.h"

namespace rmscore {
namespace modernapi {

class SealPolicy;

class DLL_PUBLIC_RMS SealPolicy {

public:

    /**
   * @brief Gets a new SealPolicy to work with
   * @param AuthenticationCallback - Handles OAuth requests when obtaining signing certificate
   * @param UserEmail - Email of user signing the document
   * @param ClientID - Provided by application, used for authentication
   * @return
   */
  static std::shared_ptr<SealPolicy> Acquire(
          IAuthenticationCallback& AuthenticationCallback,
          std::string& UserEmail,
          std::string& ClientID);

  /**
   * @brief Authenticates if the signature attached to the seal matches the data of the seal itself
   * @param seal
   * @return
   */
  static bool VerifySeal(Seal seal);

  /**
   * @brief Inserts data into the seal
   * @param Key
   * @param Value
   */
  void AddContent(std::string& Key, std::string& Value);

  /**
   * @brief Creates a signed seal for the user
   * @return
   */
  Seal GenerateSeal();

private:

  SealPolicy();

  Seal seal;

};

} // namespace modernapi
} // namespace rmscore
#endif // SEALPOLICY_H
