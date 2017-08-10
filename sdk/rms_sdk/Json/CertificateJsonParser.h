#ifndef CERTIFICATEJSONPARSER_H
#define CERTIFICATEJSONPARSER_H

#include "Platform/Json/IJsonObject.h"
#include "Common/Constants.h"
#include "Common/tools.h"

using namespace rmscore::platform::json;
using namespace rmscore::common;
using namespace std;

namespace rmscore {
namespace json {


/**
 * @brief The CertificateJsonParser class takes in a JSON object representing a certificate,
 * and pulls out the public key, private key, and exponent.
 */
class CertificateJsonParser
{
public:
    CertificateJsonParser(shared_ptr<IJsonObject> certificateJSON);
    vector<uint8_t> GetPublicModulus();
    vector<uint8_t> GetPublicExponent();
    vector<uint8_t> GetPrivateModulus();

    /**
     * @brief GetPublicCertificate creates a new JSON certificate by duplicating the existing certificate, but does not
     * copy over the private key data to the new certificate.
     *
     * This is used when generating the signature. The CLC's public portion needs to be attached to the signature, but the private
     * key should not be.
     *
     * @return
     */
    shared_ptr<IJsonObject> GetPublicCertificate();

private:
    shared_ptr<IJsonObject> GetPayload();
    shared_ptr<IJsonObject> GetPublicKey();
    shared_ptr<IJsonObject> certificate;
};


}
}

#endif // CERTIFICATEJSONPARSER_H
