#ifndef SEALSIGNER_H
#define SEALSIGNER_H

#include "IStringSigner.h"
#include "Json/CertificateJsonParser.h"
#include "CryptoAPI.h"

using namespace rmscore::json;

namespace rmscore{
namespace core{

 /**
 * @brief The StringSigner class implements the StringSigner interface
 */
class StringSigner : public IStringSigner
{

friend class IStringSigner;

//public:
    /**
     * @brief GenerateSignature gets the Seal's Data hash using GetDataHash(), and signs it with the certificate provided
     * in the constructor
     * @param seal - Seal to Sign
     * @return Signature object
     */
    virtual Signature GenerateSignature(std::string& data) override;

private:
    /**
     * @brief Constructor
     * @param Certificate - JSON object.
     */
    StringSigner(std::shared_ptr<IJsonObject> Certificate);
    CertificateJsonParser certificateJsonParser;
    std::shared_ptr<rmscore::platform::json::IJsonObject> certificate;
    std::shared_ptr<rmscrypto::api::IRSAKeyBlob> rsaKeyBlob;

};
}
}

#endif // SEALSIGNER_H
