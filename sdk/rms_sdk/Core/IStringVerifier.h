#ifndef ISTRINGVERIFIER_H
#define ISTRINGVERIFIER_H

#include "Platform/Json/IJsonObject.h"

using namespace rmscore::platform::json;

namespace rmscore{
namespace core{

/**
 * @brief The IStringVerifier class verifies if a hash is equal to a digest signed by a certificate
 */
class IStringVerifier{

public:

    /**
     * @brief Create a new StringVerifier
     * @param certificate
     * @return
     */
    static std::shared_ptr<IStringVerifier> GetStringVerifier(std::shared_ptr<IJsonObject> certificate);

    /**
     * @brief Checks if the hash is simply the digest signed by the certificate passed in the constructor
     * @param hash
     * @param digest
     * @return
     */
    virtual bool VerifyString(std::string& hash, std::string& digest) = 0;

};

}
}

#endif // ISTRINGVERIFIER_H
