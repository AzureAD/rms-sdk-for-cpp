#include "RSAKeyBlob.h"
#include "../../Crypto/CryptoConstants.h"
#include "../../../rms_sdk/Common/tools.h"
#include "../../../rms_sdk/ModernAPI/RMSExceptions.h"
#include <openssl/sha.h>
#include <openssl/err.h>
#include <openssl/objects.h>

using namespace std;
using namespace rmscrypto::crypto;
using namespace rmscrypto::api;
using namespace rmscore;

namespace rmscrypto {
namespace platform {
namespace crypto {

//IMPORTANT
//you must base64 decode data before calling this ctor
//publicKey and privateKey will be reversed when this function is called because the server uses little-endian and openssl uses big-endian
//exponent will not be reversed and should look like "65537" after being converted to a string
//if checkKey is set to true, p, q, dmp1, dmq1, and impq will all be calculated and RSA_check_key will be called.

//TODO: cache p and q to make signing faster
RSAKeyBlob::RSAKeyBlob(std::vector<uint8_t> privateKey, std::vector<uint8_t> exponent, std::vector<uint8_t> publicKey, bool checkKey)
{
    string exp(exponent.begin(), exponent.end());
    std::reverse(privateKey.begin(), privateKey.end());
    std::reverse(publicKey.begin(), publicKey.end());

    BIGNUM* d = NULL;
    d = BN_bin2bn(&privateKey[0], privateKey.size(), NULL);
    BIGNUM* e = BN_new();
    BN_dec2bn(&e, exp.c_str());
    BIGNUM* n = NULL;
    n = BN_bin2bn(&publicKey[0], publicKey.size(), NULL);

    if (checkKey)
        this->m_rsa = CreateAndCheck(d, n, e);
    else
        this->m_rsa = CreateNoCheck(d, n, e);
}

shared_ptr<RSA> RSAKeyBlob::CreateNoCheck(BIGNUM *d, BIGNUM *n, BIGNUM* e)
{
    shared_ptr<RSA> rsa(RSA_new(), [](RSA* r){ RSA_free(r); });
    rsa->d = d;
    rsa->n = n;
    rsa->e = e;
    return rsa;
}

shared_ptr<RSA> RSAKeyBlob::CreateAndCheck(BIGNUM *d, BIGNUM *n, BIGNUM* e)
{
    //note to reader: don't modify this function. it works. trust me.
    shared_ptr<RSA> rsa(RSA_new(), [](RSA* r){ RSA_free(r); });

    BIGNUM* TWO = BN_new();
    BN_set_word(TWO, 2);
    BIGNUM* ONE = BN_new();
    BN_one(ONE);

    rsa->d = d;
    rsa->n = n;
    rsa->e = e;
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* result = BN_new();
    // k = (d * e) - 1
    BN_mul(result, d, e, ctx);
    BN_sub_word(result, 1);
    if (!BN_is_odd(result))
    {
        BIGNUM* r = result;
        BIGNUM* t = BN_new();
        BN_zero(t);

        do {
            // r = r / 2
            // t++;
            BN_div(r, NULL, r, TWO, ctx);
            BN_add(t, t, ONE);
        } while (!BN_is_odd(r));

        BIGNUM* y = BN_new();
        BIGNUM* nmo = BN_new();
        BIGNUM* g = BN_new();
        BIGNUM* x = BN_new();
        BIGNUM* tmo = BN_new();
        BN_sub(nmo, n, ONE);
        for (int i = 0; i < 100; i++)
        {
STEP3:
            do
            {
                BN_rand_range(g, nmo);
                BN_mod_exp(y, g, r, n, ctx);
            } while (BN_cmp(y, ONE) == 0 || BN_cmp(y, nmo) == 0);
            BN_sub(tmo, tmo, ONE);
            for (BIGNUM* j = ONE; BN_cmp(j, tmo) <= 0; BN_add_word(j, 1))
            {
                BN_mod_exp(x, y, TWO, n, ctx);
                if (BN_cmp(x, ONE) == 0)
                    goto STEP5;
                if (BN_cmp(x, nmo) == 0)
                    goto STEP3;
                y = x;
            }
            BN_mod_exp(x, y, TWO, n, ctx);
            if (BN_cmp(x, ONE) == 0)
                goto STEP5;
        }
STEP5:
        BN_sub_word(y, 1);
        BIGNUM* p = BN_new();
        BN_gcd(p, y, n, ctx);
        BIGNUM* q = BN_new();
        BN_div(q, NULL, n, p, ctx);
        BN_mul(result, p, q, ctx);

        if (BN_cmp(n, result) != 0)
        {
            throw exceptions::RMSCryptographyException("Incorrect calculation of p and q");
        }
        rsa->p = p;
        rsa->q = q;

        rsa->dmp1 = BN_new();
        BN_sub(result, p, ONE);
        BN_mod(rsa->dmp1, d, result, ctx);

        rsa->dmq1 = BN_new();
        BN_sub(result, q, ONE);
        BN_mod(rsa->dmq1, d, result, ctx);

        rsa->iqmp = BN_new();
        BN_mod_inverse(rsa->iqmp, q, p, ctx);

        common::operate_many(BN_free, result, TWO, ONE, x, y, tmo, g, nmo);

        BN_CTX_free(ctx);

        auto ret = RSA_check_key(rsa.get());
        if (ret != 1)
        {
            throw exceptions::RMSCryptographyException("RSA_check_key failed. Error: " + string(ERR_error_string(ERR_get_error(), NULL)));
        }

        return rsa;
    }
    common::operate_many(BN_free, ONE, TWO);
    BN_CTX_free(ctx);
    throw exceptions::RMSCryptographyException("No prime factorization of given RSA public key");
}

vector<uint8_t> RSAKeyBlob::Sign(std::vector<uint8_t> digest, uint32_t& outSize)
{
    uint32_t retSize;
    auto rsaSize = RSA_size(m_rsa.get());
    vector<uint8_t> sigBuf(rsaSize);
    int32_t ret = RSA_sign(NID_sha1, &digest[0], digest.size(), &sigBuf[0], &retSize, m_rsa.get());
    if (ret != 1)
        throw exceptions::RMSCryptographyException("RSA_sign failed. Error: " + string(ERR_error_string(ERR_get_error(), NULL)));
    outSize = retSize;
    return sigBuf;
}

bool RSAKeyBlob::VerifySignature(std::vector<uint8_t> signature, std::vector<uint8_t> digest, string& outMsg, uint32_t retsize)
{
    auto ret = RSA_verify(NID_sha1, &digest[0], digest.size(), &signature[0], retsize, m_rsa.get());
    if (ret != 1)
    {
        outMsg = "RSA_verify returned false. Error: " + string(ERR_error_string(ERR_get_error(), NULL));
        return false;
    }
    return true;
}

vector<uint8_t> RSAKeyBlob::PublicEncrypt(std::vector<uint8_t> buffer)
{
    auto size = RSA_size(m_rsa.get());
    vector<uint8_t> encryptedKeyBuf(size);
    int result = RSA_public_encrypt(buffer.size(), &buffer[0], &encryptedKeyBuf[0], m_rsa.get(), RSA_PKCS1_PADDING);
    if (result == -1)
    {
        string ret;
        unsigned long code;
        do {
            code = ERR_get_error();
            ret += string(ERR_error_string(code, NULL)) + '\n';
        } while (code != 0);
        throw exceptions::RMSCryptographyException("Failed to RSA encrypt session key: " + ret);
    }
    else if (result != size)
        throw exceptions::RMSCryptographyException("RSA_public_encrypt returned unexpected size.");
    return encryptedKeyBuf;
}

vector<uint8_t> RSAKeyBlob::PrivateDecrypt(std::vector<uint8_t> buffer)
{
    auto size = RSA_size(m_rsa.get());
    vector<uint8_t> decryptedKeyBuf(size);
    int result = RSA_private_decrypt(buffer.size(), &buffer[0], &decryptedKeyBuf[0], m_rsa.get(), RSA_PKCS1_PADDING);
    if (result == -1)
    {
        string ret;
        unsigned long code;
        do {
            code = ERR_get_error();
            ret += string(ERR_error_string(code, NULL)) + '\n';
        } while (code != 0);
        throw exceptions::RMSCryptographyException("Failed to RSA encrypt session key: " + ret);
    }
    else if (result != size)
        throw exceptions::RMSCryptographyException("RSA_public_encrypt returned unexpected size.");
    return decryptedKeyBuf;
}
} //crypto
} //platform
} //rmscrypto
