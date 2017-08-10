#ifndef STRINGVERIFIER_H
#define STRINGVERIFIER_H

#include "IStringVerifier.h"
#include "Json/CertificateJsonParser.h"
#include "Common/tools.h"
#include "CryptoAPI.h"

using namespace rmscore::json;

namespace rmscore {
namespace core {

/**
 * @brief The StringVerifier class implements the IStringVerifier interface
 */
class StringVerifier : public IStringVerifier
{

friend class IStringVerifier;

public:
     virtual bool VerifyString(std::string& hash, std::string& digest) override;

private:
    StringVerifier(std::shared_ptr<IJsonObject> certificate);
    CertificateJsonParser certificateJsonParser;
};


}
}

#endif // STRINGVERIFIER_H
