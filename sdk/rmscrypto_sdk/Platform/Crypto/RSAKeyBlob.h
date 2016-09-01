#ifndef RSACRYPTOKEY_H
#define RSACRYPTOKEY_H
#include "../../CryptoAPI/IRSAKeyBlob.h"
#include <vector>
#include <memory>
#include "openssl/rand.h"
#include "openssl/rsa.h"

using namespace rmscrypto::api;

namespace rmscrypto {
namespace platform {
namespace crypto {
class RSAKeyBlob : public IRSAKeyBlob
{
public:
    RSAKeyBlob(std::vector<uint8_t> privateKey, std::vector<uint8_t> exponent, std::vector<uint8_t> publicKey, bool checkKey);

    virtual std::vector<uint8_t> Sign(std::vector<uint8_t> digest, uint32_t& outsize) override;

    virtual bool VerifySignature(std::vector<uint8_t> signature, std::vector<uint8_t> digest, std::string& outMsg, uint32_t retsize) override;

    virtual std::vector<uint8_t> PublicEncrypt(std::vector<uint8_t> buffer) override;

    virtual std::vector<uint8_t> PrivateDecrypt(std::vector<uint8_t> buffer) override;

    ~RSAKeyBlob() { }

private:
    std::shared_ptr<RSA> m_rsa;
    std::shared_ptr<RSA> create_and_check(BIGNUM* d, BIGNUM* n, BIGNUM* e);
    std::shared_ptr<RSA> create_no_check(BIGNUM* d, BIGNUM* n, BIGNUM* e);
};
} //crypto
} //platform
} //rmscrypto

#endif // RSACRYPTOKEY_H
