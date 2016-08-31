/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include "PublishClient.h"
#include "RestClientCache.h"
#include "RestClientErrorHandling.h"
#include "RestHttpClient.h"
#include "RestObjects.h"
#include "RestServiceUrls.h"
#include "RestServiceUrlClient.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Json/IJsonSerializer.h"
#include "../Platform/Json/IJsonObject.h"
#include "../Platform/Json/IJsonArray.h"
#include "../Platform/Json/IJsonParser.h"
#include "../Platform/Json/JsonObjectQt.h"
#include "../Common/tools.h"
#include "../Crypto/CryptoConstants.h"
#include <sstream>
#include <ostream>
#include <fstream>
#include <QDebug>
#include <functional>

using namespace std;
using namespace rmscore::common;
using namespace rmscore::json;
using namespace rmscore::platform::http;
using namespace rmscore::platform::json;
using namespace rmscrypto::api;

namespace rmscore {
namespace restclients {
const static string CLCCacheName = "CLC";
const static string CLCCacheTag = "CLIENT_LICENSOR_CERTIFICATES_UR";

PublishResponse PublishClient::PublishUsingTemplate(
  const PublishUsingTemplateRequest     & request,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const std::string                       sEmail,
  std::shared_ptr<std::atomic<bool> >     cancelState)
{
  auto pJsonSerializer   = IJsonSerializer::Create();
  auto serializedRequest = pJsonSerializer->SerializePublishUsingTemplateRequest(
    request);

  return PublishCommon(move(
                         serializedRequest), authenticationCallback, sEmail,
                       cancelState);
}

PublishResponse PublishClient::LocalPublishUsingTemplate(
  const PublishUsingTemplateRequest     & request,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const std::string                       sEmail,
  std::shared_ptr<std::atomic<bool> >     cancelState,
  const std::function<std::string(std::string, std::string&)>& getCLCCallback)
{
    auto response = PublishResponse();
    auto pJsonRoot = IJsonObject::Create();
    auto pPayload = IJsonObject::Create();

    string clcPubData;
    string sCLC;
    if (getCLCCallback == nullptr)
        sCLC = GetCLC(sEmail, authenticationCallback, cancelState, clcPubData);
    else
        sCLC = getCLCCallback(sEmail, clcPubData);
    auto bytearray = common::ByteArray(sCLC.begin(), sCLC.end());

    auto pCLC =  IJsonParser::Create()->Parse(bytearray);
    auto pClcPld = pCLC->GetNamedObject("pld");
    auto pClcPubData = pClcPld->GetNamedObject("pub");
    auto pClcPubPayload = pClcPubData->GetNamedObject("pld");
    auto pClcIssuedTo = pClcPubPayload->GetNamedObject("issto");
    auto pClcIssuer = pClcPubPayload->GetNamedObject("iss");
    auto pServerPubRsaKey = pClcIssuer->GetNamedObject("pubk");
    auto pClientPubRsaKey = pClcIssuedTo->GetNamedObject("pubk");
    auto pClientPriKey = pClcPld->GetNamedObject("pri")->GetNamedObject("prik");

    auto header = IJsonObject::Create();
    header->SetNamedString("ver", "1.0.0.0");
    pPayload->SetNamedObject("hdr", *header);

    response.contentId = common::GenerateAGuid();
    response.owner = pClcIssuedTo->GetNamedString("fname");

    auto license = IJsonObject::Create();
    license->SetNamedString("id", common::GenerateAGuid());
    license->SetNamedString("o", pClcIssuedTo->GetNamedString("fname"));
    //auto vCre = pClcPubData->Stringify();
    //string sCre(vCre.begin(), vCre.end());

    license->SetNamedString("cre", clcPubData);

    license->SetNamedString("cid", pClcIssuedTo->GetNamedString("em"));

    auto clcPrivData = pClcPld->GetNamedObject("pri");
    license->SetNamedString("lcd", clcPrivData->GetNamedString("issd"));
    license->SetNamedString("exp", clcPrivData->GetNamedString("exp"));

    auto pSignedApplicationDataJson = IJsonObject::Create();
    for_each(
      begin(request.signedApplicationData),
      end(request.signedApplicationData),
      [ = ](const pair<string, string>& appData)
        {
          pSignedApplicationDataJson->SetNamedString(appData.first,
                                                     appData.second);
        });
    if (request.signedApplicationData.size() != 0)
        license->SetNamedObject("sad", *pSignedApplicationDataJson);

    auto pEncryptedPolicy = IJsonObject::Create();
    pEncryptedPolicy->SetNamedString("crem", pClcIssuedTo->GetNamedString("em"));
    pEncryptedPolicy->SetNamedString("tid", request.templateId);

    //encrypted app data would go here

    //session key init
    const unsigned int keysize = 32;
    uint8_t* skbuf = new uint8_t[keysize];
    RAND_bytes(skbuf, keysize);
    vector<uint8_t> sessionkey;
    sessionkey.assign(skbuf, skbuf + keysize);
    delete[] skbuf;
    CipherMode cm = request.bPreferDeprecatedAlgorithms ? CIPHER_MODE_ECB : CIPHER_MODE_CBC4K;


    //encrypted policy
    auto enp = EncryptPolicyToBase64(pEncryptedPolicy, sessionkey, cm);
    license->SetNamedValue("enp", enp);

    //session key
    auto pSessionKey = IJsonObject::Create();

    uint8_t* ckbuf = new uint8_t[keysize];
    RAND_bytes(ckbuf, keysize);
    vector<uint8_t> contentkey;
    contentkey.assign(ckbuf, skbuf + keysize);
    delete[] ckbuf;

    auto pContentKey = IJsonObject::Create();
    pContentKey->SetNamedString("alg", "AES");
    pContentKey->SetNamedString("cm", "CBC4K");
    pContentKey->SetNamedValue("k", common::ConvertBytesToBase64(contentkey)); //not sure if correct
    auto bytes = common::ConvertBytesToBase64(pContentKey->Stringify());
    pSessionKey->SetNamedObject("eck", *pContentKey);

    pSessionKey->SetNamedString("eck", std::string(bytes.begin(), bytes.end()));
    pSessionKey->SetNamedString("alg", "AES");
    pSessionKey->SetNamedString("cm", request.bPreferDeprecatedAlgorithms ? "ECB" : "CBC4K");
    //encrypt session key with public RSA key from CLC
    auto e = pClientPubRsaKey->GetNamedString("e");
    auto n = pClientPubRsaKey->GetNamedString("n");
    common::ByteArray sn(n.begin(),n.end());
    common::ByteArray se(e.begin(), e.end());
    common::ByteArray bn = common::ConvertBase64ToBytes(sn);
    auto esk = RSAEncryptKey(se, bn, sessionkey);

    pSessionKey->SetNamedString("sk", esk);

    license->SetNamedObject("sk", *pSessionKey);

    pPayload->SetNamedObject("lic", *license);

    auto vPld = pPayload->Stringify();

    size_t size;
    auto toHash = Reformat(vPld, 1);

    string sToHash(toHash.begin(), toHash.end());
    auto digest = common::HashString(toHash, &size, false);
    toHash = Escape(toHash);
    sToHash = string(toHash.begin(), toHash.end());

    auto d = pClientPriKey->GetNamedString("d");

    e = pClientPubRsaKey->GetNamedString("e");
    n = pClientPubRsaKey->GetNamedString("n");
    string sSig = R"("sig":{"alg":"SHA1", "penc":"utf-8", "dig":")" + RSASignPayload(d, n, e, digest) + "\"}}";
    string sFinal = R"({"pld":")" + sToHash + R"(",)" + sSig;
    vector<uint8_t> vFinal(sFinal.begin(), sFinal.end());

    //add byte-order marker
    vector<uint8_t> UTF8bom = { 0xEF, 0xBB, 0xBF };
    vFinal.insert(vFinal.begin(), UTF8bom.begin(), UTF8bom.end());
    response.serializedLicense = vFinal;

    auto responseKey = KeyDetailsResponse();
    responseKey.algorithm = "AES";
    responseKey.cipherMode = ICryptoProvider::CipherModeString(cm);
    responseKey.value = common::ConvertBytesToBase64(contentkey);
    response.key = responseKey;
    response.signedApplicationData = request.signedApplicationData;
    response.name = "Restricted Access";
    response.description = "Permission is currently restricted. Only specified users can access this content.";
    return response;
}

shared_ptr<CLCCacheResult> PublishClient::GetCLCCache(shared_ptr<IRestClientCache> cache, const std::string& email)
{
    shared_ptr<CLCCacheResult> result;
    size_t size;
    vector<uint8_t> pKey = common::HashString(vector<uint8_t>(email.begin(), email.end()), &size);
    auto clc = cache->Lookup(CLCCacheName, CLCCacheTag, &pKey[0], size, true);
    if (clc.capacity() > 0)
        result = make_shared<CLCCacheResult>(clc.at(0), false);
    else
        result = make_shared<CLCCacheResult>("", true);
    return result;
}

PublishResponse PublishClient::PublishCustom(
  const PublishCustomRequest            & request,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const std::string                       sEmail,
  std::shared_ptr<std::atomic<bool> >     cancelState)
{
  auto pJsonSerializer   = IJsonSerializer::Create();
  auto serializedRequest =
    pJsonSerializer->SerializePublishCustomRequest(request);

  return PublishCommon(move(
                         serializedRequest), authenticationCallback, sEmail,
                       cancelState);
}

PublishResponse PublishClient::LocalPublishCustom(
  const PublishCustomRequest            & request,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const std::string                       sEmail,
  std::shared_ptr<std::atomic<bool> >     cancelState,
  const std::function<std::string(std::string, std::string&)>& getCLCCallback)
{
    auto response = PublishResponse();
    auto pJsonRoot = IJsonObject::Create();
    auto pPayload = IJsonObject::Create();

    string clcPubData;
    string sCLC;
    if (getCLCCallback == nullptr)
        sCLC = GetCLC(sEmail, authenticationCallback, cancelState, clcPubData);
    else
        sCLC = getCLCCallback(sEmail, clcPubData);
    auto bytearray = common::ByteArray(sCLC.begin(), sCLC.end());

    auto pCLC =  IJsonParser::Create()->Parse(bytearray);
    auto pClcPld = pCLC->GetNamedObject("pld");
    auto pClcPubData = pClcPld->GetNamedObject("pub");
    auto pClcPubPayload = pClcPubData->GetNamedObject("pld");
    auto pClcIssuedTo = pClcPubPayload->GetNamedObject("issto");
    auto pClcIssuer = pClcPubPayload->GetNamedObject("iss");
    auto pServerPubRsaKey = pClcIssuer->GetNamedObject("pubk");
    auto pClientPubRsaKey = pClcIssuedTo->GetNamedObject("pubk");
    auto pClientPriKey = pClcPld->GetNamedObject("pri")->GetNamedObject("prik");

    auto header = IJsonObject::Create();
    header->SetNamedString("ver", "1.0.0.0");
    pPayload->SetNamedObject("hdr", *header);

    response.contentId = common::GenerateAGuid();
    response.owner = pClcIssuedTo->GetNamedString("fname");

    auto license = IJsonObject::Create();
    license->SetNamedString("id", common::GenerateAGuid());
    license->SetNamedString("o", pClcIssuedTo->GetNamedString("fname"));
    //auto vCre = pClcPubData->Stringify();
    //string sCre(vCre.begin(), vCre.end());

    license->SetNamedString("cre", clcPubData);

    license->SetNamedString("cid", pClcIssuedTo->GetNamedString("em"));

    auto clcPrivData = pClcPld->GetNamedObject("pri");
    license->SetNamedString("lcd", clcPrivData->GetNamedString("issd"));
    license->SetNamedString("exp", clcPrivData->GetNamedString("exp"));

    auto pSignedApplicationDataJson = IJsonObject::Create();
    for_each(
      begin(request.signedApplicationData),
      end(request.signedApplicationData),
      [ = ](const pair<string, string>& appData)
        {
          pSignedApplicationDataJson->SetNamedString(appData.first,
                                                     appData.second);
        });
    if (request.signedApplicationData.size() != 0)
        license->SetNamedObject("sad", *pSignedApplicationDataJson);

    auto pEncryptedPolicy = IJsonObject::Create();
    pEncryptedPolicy->SetNamedString("crem", pClcIssuedTo->GetNamedString("em"));
    //encrypted app data would go here
    auto rights = ConvertUserRights(request);
    auto pCustomPolicy = IJsonObject::Create();
    pCustomPolicy->SetNamedArray("usrts", *rights);
    pEncryptedPolicy->SetNamedObject("cp", *pCustomPolicy);

    //session key init
    const unsigned int keysize = AES_BLOCK_SIZE;
    uint8_t* skbuf = new uint8_t[keysize];
    RAND_bytes(skbuf, keysize);
    vector<uint8_t> sessionkey(skbuf, skbuf + keysize);
    CipherMode cm = request.bPreferDeprecatedAlgorithms ? CIPHER_MODE_ECB : CIPHER_MODE_CBC4K;

    //encrypted policy
    auto enp = EncryptPolicyToBase64(pEncryptedPolicy, sessionkey, cm);
    license->SetNamedValue("enp", enp);

    //session key
    auto pSessionKey = IJsonObject::Create();

    uint8_t* ckbuf = new uint8_t[keysize];
    RAND_bytes(ckbuf, keysize);
    vector<uint8_t> contentkey(ckbuf, ckbuf + keysize);

    auto pContentKey = IJsonObject::Create();
    pContentKey->SetNamedString("alg", "AES");
    pContentKey->SetNamedString("cm", "CBC4K");
    pContentKey->SetNamedValue("k", common::ConvertBytesToBase64(contentkey));
    auto bytes = common::ConvertBytesToBase64(pContentKey->Stringify());
    pSessionKey->SetNamedObject("eck", *pContentKey);

    pSessionKey->SetNamedString("eck", std::string(bytes.begin(), bytes.end()));
    pSessionKey->SetNamedString("alg", "AES");
    pSessionKey->SetNamedString("cm", request.bPreferDeprecatedAlgorithms ? "ECB" : "CBC4K");
    //encrypt session key with public RSA key from CLC
    auto e = pClientPubRsaKey->GetNamedString("e");
    auto n = pClientPubRsaKey->GetNamedString("n");
    common::ByteArray sn(n.begin(),n.end());
    common::ByteArray se(e.begin(), e.end());
    common::ByteArray bn = common::ConvertBase64ToBytes(sn);
    auto esk = RSAEncryptKey(se, bn, sessionkey);

    pSessionKey->SetNamedString("sk", esk);

    license->SetNamedObject("sk", *pSessionKey);

    pPayload->SetNamedObject("lic", *license);

    auto vPld = pPayload->Stringify();

    size_t size;
    auto toHash = Reformat(vPld, 1);

    string sToHash(toHash.begin(), toHash.end());
    auto digest = common::HashString(toHash, &size, false);
    toHash = Escape(toHash);
    sToHash = string(toHash.begin(), toHash.end());

    auto k = pClientPriKey->GetNamedString("d");
    e = pClientPubRsaKey->GetNamedString("e");
    n = pClientPubRsaKey->GetNamedString("n");
    string sSig = R"("sig":{"alg":"SHA1", "penc":"utf-8", "dig":")" + RSASignPayload(k, n, e, digest) + "\"}}";
    string sFinal = R"({"pld":")" + sToHash + R"(",)" + sSig;
    vector<uint8_t> vFinal(sFinal.begin(), sFinal.end());

    //add byte-order marker
    vector<uint8_t> UTF8bom = { 0xEF, 0xBB, 0xBF };
    vFinal.insert(vFinal.begin(), UTF8bom.begin(), UTF8bom.end());
    response.serializedLicense = vFinal;

    auto responseKey = KeyDetailsResponse();
    responseKey.algorithm = "AES";
    responseKey.cipherMode = ICryptoProvider::CipherModeString(cm);
    responseKey.value = common::ConvertBytesToBase64(contentkey);
    response.key = responseKey;
    response.signedApplicationData = request.signedApplicationData;
    response.name = "Restricted Access";
    response.description = "Permission is currently restricted. Only specified users can access this content.";

    delete[] skbuf;
    delete[] ckbuf;

    return response;
}

PublishResponse PublishClient::PublishCommon(
  common::ByteArray                    && requestBody,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const std::string                     & sEmail,
  std::shared_ptr<std::atomic<bool> >     cancelState)
{
  auto pRestServiceUrlClient = RestServiceUrlClient::Create();
  auto publishUrl            = pRestServiceUrlClient->GetPublishUrl(sEmail,
                                                                    authenticationCallback,
                                                                    cancelState);

  auto result = RestHttpClient::Post(
    publishUrl,
    move(requestBody),
    authenticationCallback,
    cancelState);

  if (StatusCode::OK != result.status)
  {
    HandleRestClientError(result.status, result.responseBody);
  }

  auto pJsonSerializer = IJsonSerializer::Create();
  try
  {
    //    size_t respSize = result.responseBody.size();
    return pJsonSerializer->DeserializePublishResponse(result.responseBody);
  }
  catch (exceptions::RMSException)
  {
    throw exceptions::RMSNetworkException(
            "PublishClient: Got an invalid json from the REST service.",
            exceptions::RMSNetworkException::ServerError);
  }
}

common::ByteArray PublishClient::Escape(common::ByteArray source)
{
    string op(source.begin(), source.end());
    string ret;
    for (uint32_t i = 0; i < op.size(); i++)
    {
        if (op[i] == '\\')
            ret += R"(\\)";
        else if (op[i] == '"')
            ret += R"(\")";
        else
            ret += op[i];
    }
    return ByteArray(ret.begin(), ret.end());
}

common::ByteArray PublishClient::Reformat(common::ByteArray source, int currentlevel)
{
    string ret(source.begin(), source.end());
    if (currentlevel != 1)
    {
        auto pos = ret.find(R"(\\\"sig\\\")");
        if (pos != std::string::npos)
        {
            ret = common::ReplaceString(ret, R"(\"{\\\"pld\\\":)", R"({\"pld\":\")");
            pos = ret.find(R"(\\\"sig\\\")");

            string final = ret.substr(0, pos - 1);
            final += R"(\",)";

            bool end = false;
            uint32_t it = pos;
            while (!end)
            {
                string s = ret.substr(it, 2);
                if (s == "\\\\")
                    final += "";
                else
                    final += s;

                if (s == "}}")
                    end = true;
                it += 2;
            }
            final += ret.substr(it + 2);
            return vector<uint8_t>(final.begin(), final.end());
        }
        return vector<uint8_t>(ret.begin(), ret.end());
    }
    else
    {
        ret = common::ReplaceString(ret, R"("{\"pld\":)", R"({"pld":")");
        auto pos = ret.find(R"(\"sig\")");
        if (pos == std::string::npos)
            throw exceptions::RMSInvalidArgumentException("source");

        string final = ret.substr(0, pos - 1);
        final += R"(",)";

        bool end = false;
        uint32_t it = pos;
        while (!end)
        {
            string s = ret.substr(it, 1);
            if (s == "\\")
                final += "";
            else
                final += s;

            if (ret.substr(it, 2) == "}}")
                end = true;
            ++it;
        }
        final += "}" + ret.substr(it + 2);
        return vector<uint8_t>(final.begin(), final.end());
    }
}

common::ByteArray PublishClient::EncryptPolicyToBase64(std::shared_ptr<IJsonObject> pPolicy, vector<uint8_t> key, CipherMode cm)
{
    auto crypto = CreateCryptoProvider(cm, key);
    vector<uint8_t> encryptedPolicyBytes = pPolicy->Stringify();

    auto totalLen = encryptedPolicyBytes.size();

    const bool CBC4K = cm == CIPHER_MODE_CBC4K;

    unsigned int blockSize = CBC4K ? rmscrypto::crypto::CBC4K_BLOCK_SIZE : rmscrypto::crypto::AES128_BLOCK_SIZE;
    vector<vector<uint8_t>> blockList((totalLen / blockSize) + 1);

    for (unsigned int i = 0; i < blockList.size(); i++)
    {
        bool isFinal = i == blockList.size() - 1;
        if (!isFinal)
            blockList[i] = vector<uint8_t>(encryptedPolicyBytes.begin() + (i * blockSize), encryptedPolicyBytes.begin() + ((i + 1) * blockSize));
        else
        {
            vector<uint8_t> finalBlock(encryptedPolicyBytes.begin() + (i * blockSize), encryptedPolicyBytes.end());
            blockList[i] = finalBlock;
        }
    }

    vector<uint8_t> final;

    auto len = totalLen;

    const unsigned int PADDED_CBC4K_SIZE = 4412;

    for (unsigned int i = 0; i < blockList.size(); i++)
    {
        vector<uint8_t> outbuffer;
        vector<uint8_t> inputbuffer = blockList[i];
        uint32_t _size = inputbuffer.size();

        uint32_t byteswritten = 0;
        bool isLastBlock = len < blockSize;
        if (CBC4K)
        {
            if (!isLastBlock)
                outbuffer = vector<uint8_t>(blockSize, 0);
            else
                outbuffer = vector<uint8_t>(PADDED_CBC4K_SIZE, 0);
        }
        crypto->Encrypt(&inputbuffer[0], blockSize, i, isLastBlock, &outbuffer[0], outbuffer.size(), &byteswritten);
        uint32_t actualEncryptedBytes = _size - (_size % 16) + 16;
        final.insert(
          final.end(),
          outbuffer.begin(),
          outbuffer.begin() + (_size < rmscrypto::crypto::CBC4K_BLOCK_SIZE ? actualEncryptedBytes : PADDED_CBC4K_SIZE));
        len -= blockSize;
    }
    return common::ConvertBytesToBase64(final);
}

bool PublishClient::Even(BIGNUM *num)
{
    auto res = BN_mod_word(num, 2);
    return res == 0;
}

shared_ptr<RSA> PublishClient::CreateRSA(BIGNUM *d, BIGNUM *n, BIGNUM* e)
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
        BN_free(result);
        BN_free(TWO);
        BN_free(ONE);
        BN_free(x);
        BN_free(y);
        BN_free(tmo);
        BN_free(g);
        BN_free(nmo);
        BN_CTX_free(ctx);

        auto ret = RSA_check_key(rsa.get());
        if (ret != 1)
        {
            throw exceptions::RMSCryptographyException("RSA_sign failed. Error: " + string(ERR_error_string(ERR_get_error(), NULL)));
        }

        return rsa;
    }
    BN_free(result);
    BN_free(TWO);
    BN_free(ONE);
    BN_CTX_free(ctx);
    throw exceptions::RMSCryptographyException("No prime factorization of given RSA public key");
}

string PublishClient::RSAEncryptKey(const ByteArray& exp, const ByteArray& mod, vector<uint8_t> buf)
{
    RSA* rsa = RSA_new();
    BIGNUM* exponent = NULL;
    BN_dec2bn(&exponent, reinterpret_cast<const char*>(&exp[0]));
    BIGNUM* modulus_ = NULL;
    /*
        Convert to big endian.
    */
    vector<uint8_t> mod_reverse(mod.size());

    std::reverse_copy(mod.begin(), mod.end(), mod_reverse.begin());

    modulus_ = BN_bin2bn(&mod_reverse[0], mod_reverse.size(), NULL);

    rsa->e = exponent;
    rsa->n = modulus_;

    auto size = RSA_size(const_cast<const RSA*>(rsa));
    vector<uint8_t> encryptedKeyBuf(size);
    int result = RSA_public_encrypt(buf.size(), &buf[0], &encryptedKeyBuf[0], rsa, RSA_PKCS1_PADDING);
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
    vector<uint8_t> vEsk = common::ConvertBytesToBase64(encryptedKeyBuf);
    string sEsk(vEsk.begin(), vEsk.end());

    RSA_free(rsa);

    return sEsk;
}

string PublishClient::RSASignPayload(std::string& sPrivatekey, std::string& sPublickey, std::string& exponent, std::vector<uint8_t> digest)
{
    auto decodedPrivateKey = common::ConvertBase64ToBytes(vector<uint8_t>(sPrivatekey.begin(), sPrivatekey.end()));
    auto decodedPublicKey = common::ConvertBase64ToBytes(vector<uint8_t>(sPublickey.begin(), sPublickey.end()));

    vector<uint8_t> rDecodedPrivateKey(decodedPrivateKey.size());
    std::reverse_copy(decodedPrivateKey.begin(), decodedPrivateKey.end(), rDecodedPrivateKey.begin());

    vector<uint8_t> rDecodedPublicKey(decodedPublicKey.size());
    std::reverse_copy(decodedPublicKey.begin(), decodedPublicKey.end(), rDecodedPublicKey.begin());

    BIGNUM* d = NULL;
    d = BN_bin2bn(&rDecodedPrivateKey[0], rDecodedPrivateKey.size(), NULL);
    BIGNUM* e = BN_new();
    BN_dec2bn(&e, exponent.c_str());
    BIGNUM* n = NULL;
    n = BN_bin2bn(&rDecodedPublicKey[0], rDecodedPublicKey.size(), NULL);

    auto rsa = CreateRSA(d, n, e);

    int32_t ret;
    auto size = RSA_size(const_cast<const RSA*>(rsa.get()));
    vector<uint8_t> sigbuf(size);
    unsigned int retsize;
    ret = RSA_sign(NID_sha1, &digest[0], digest.size(), &sigbuf[0], &retsize, rsa.get());
    if (ret != 1)
        throw exceptions::RMSCryptographyException("RSA_sign failed. Error: " + string(ERR_error_string(ERR_get_error(), NULL)));
    ret = RSA_verify(NID_sha1, &digest[0], digest.size(), &sigbuf[0], retsize, rsa.get());
    if (ret != 1)
        throw exceptions::RMSCryptographyException("RSA_verify returned false. Error: " + string(ERR_error_string(ERR_get_error(), NULL)));

    sigbuf = common::ConvertBytesToBase64(sigbuf);

    return string(sigbuf.begin(), sigbuf.end());
}

std::shared_ptr<IJsonArray> PublishClient::ConvertUserRights(const PublishCustomRequest &request)
{
    auto userts = IJsonArray::Create();
    for (unsigned int i = 0; i < request.userRightsList.size(); i++)
    {
        auto _user = IJsonObject::Create();
        auto _usrs = IJsonArray::Create();
        auto _rghts = IJsonArray::Create();
        UserRightsRequest curr = request.userRightsList[i];
        for (unsigned int j = 0; j < curr.users.size(); j++)
            _usrs->Append(curr.users[j]);
        for (unsigned int j = 0; j < curr.rights.size(); j++)
            _rghts->Append(curr.rights[j]);
        _user->SetNamedArray("usrs", *_usrs);
        _user->SetNamedArray("rghts", *_rghts);
        userts->Append(*_user);
    }
    return userts;
}

std::string PublishClient::GetCLC(const std::string& sEmail, modernapi::IAuthenticationCallbackImpl& authenticationCallback, std::shared_ptr<std::atomic<bool>> cancelState, string& outClcPubData)
{
    auto pCache = RestClientCache::Create(RestClientCache::CACHE_ENCRYPTED);
    std::string clientcert;
    auto pcacheresult = GetCLCCache(pCache, sEmail);
    if (pcacheresult->CacheMissed) //cache missed, we need to get CLC from server
    {
        auto pRestServiceUrlClient = RestServiceUrlClient::Create();
        auto clcUrl = pRestServiceUrlClient->GetClientLicensorCertificatesUrl(sEmail, authenticationCallback, cancelState);
        auto request = IJsonObject::Create();
        request->SetNamedString("SignatureEncoding", "utf-8");
        auto result = RestHttpClient::Post(clcUrl, request->Stringify(),authenticationCallback, cancelState);

        if (result.status != StatusCode::OK)
            HandleRestClientError(result.status, result.responseBody);

          /*TEST CODE*/
//        std::ifstream ifs;
//        ifs.open("/home/rms/Desktop/clc.drm", ifstream::in);
//        string str{ istreambuf_iterator<char>(ifs), istreambuf_iterator<char>() };

//        auto r = CertificateResponse();
//        r.serializedCert = str;

        auto pJsonSerializer = IJsonSerializer::Create();
        try
        {
            //get clc
            auto clc = pJsonSerializer->DeserializeCertificateResponse(result.responseBody);

            string search = R"(\"pub\":)";
            auto pos = clc.serializedCert.find(search);
            if (pos == std::string::npos)
                throw exceptions::RMSInvalidArgumentException("Invalid CLC from server.");
            int bracketcount = 1;
            outClcPubData = "{";
            for(uint32_t i = pos + search.size() + 1; i < clc.serializedCert.size(); i++)
            {
                if (bracketcount == 0)
                    break;
                if (clc.serializedCert[i] == '{')
                    ++bracketcount;
                else if (clc.serializedCert[i] == '}')
                    --bracketcount;
                outClcPubData += clc.serializedCert[i];
            }
            outClcPubData = Unescape(outClcPubData);
            clc.serializedCert = Unescape(clc.serializedCert);

            auto bytearray = common::ByteArray(clc.serializedCert.begin(), clc.serializedCert.end());

            auto pCLC =  IJsonParser::Create()->Parse(bytearray);

            auto pri = pCLC->GetNamedObject("pld")->GetNamedObject("pri");

            //cache it
            const common::ByteArray response;
            std::string exp =  pri->GetNamedString("exp");

            size_t size;
            auto key = common::HashString(vector<uint8_t>(sEmail.begin(), sEmail.end()), &size);
            pCache->Store(CLCCacheName, CLCCacheTag, &key[0], size, exp, response, true);
            clientcert = clc.serializedCert;
        }
        catch(exceptions::RMSException)
        {
            throw exceptions::RMSNetworkException("PublishClient: Recieved invalid Json from REST endpoint.", exceptions::RMSNetworkException::ServerError);
        }
    }
    else
    {
        clientcert = pcacheresult->CLC;
    }
    return clientcert;
}

string PublishClient::Unescape(string source, bool skipReformat)
{
    stringstream ss;
    for (size_t i = 0; i < source.length(); i++)
        if (source[i] != '\\')
            ss << source[i];
    auto ret = ss.str();
    if (!skipReformat)
    {
        ret = common::ReplaceString(ret, "\"{\"", "{\"");
        ret = common::ReplaceString(ret, "}}\"", "}}");
    }
    return ret;
}

shared_ptr<IPublishClient>IPublishClient::Create()
{
  return make_shared<PublishClient>();
}
} // namespace restclients
} // namespace rmscore
