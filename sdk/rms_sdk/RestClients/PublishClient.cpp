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

using namespace std;
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
  std::shared_ptr<std::atomic<bool> >     cancelState)
{
    auto response = PublishResponse();
    auto pJsonRoot = IJsonObject::Create();
    auto pPayload = IJsonObject::Create();

    auto sCLC = GetCLC(authenticationCallback, sEmail, cancelState);
    auto bytearray = common::ByteArray(sCLC.begin(), sCLC.end());

    auto pCLC =  IJsonParser::Create()->Parse(bytearray);
    auto pClcPld = pCLC->GetNamedObject("pld");
    auto pClcPubData = pClcPld->GetNamedObject("pub");
    auto pClcPubPayload = pClcPubData->GetNamedObject("pld");
    auto pClcIssuedTo = pClcPubPayload->GetNamedObject("issto");
    auto pClcIssuer = pClcPubPayload->GetNamedObject("iss");
    auto pServerPubRsaKey = pClcIssuer->GetNamedObject("pubk");
    auto pClientPriKey = pClcPld->GetNamedObject("pri")->GetNamedObject("prik");

    auto header = IJsonObject::Create();
    header->SetNamedString("ver", "1");
    pPayload->SetNamedObject("hdr", *header);

    response.contentId = common::GenerateAGuid();
    response.owner = pClcIssuedTo->GetNamedString("fname");

    auto license = IJsonObject::Create();
    license->SetNamedString("id", common::GenerateAGuid());
    license->SetNamedString("o", pClcIssuedTo->GetNamedString("fname"));
    auto vCre = pClcPubData->Stringify();
    string sCre(vCre.begin(), vCre.end());
    license->SetNamedString("cre", sCre);

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
    auto e = pServerPubRsaKey->GetNamedString("e").c_str();
    auto n = pServerPubRsaKey->GetNamedString("n").c_str();
    auto esk = RSAEncryptKey(e, n, sessionkey);

    pSessionKey->SetNamedString("sk", esk);

    license->SetNamedObject("sk", *pSessionKey);

    pPayload->SetNamedObject("lic", *license);

    //sig
    auto pSig = IJsonObject::Create();
    auto vPld = pPayload->Stringify();
    string sPldEscaped(vPld.begin(), vPld.end());//= EscapeJson(pPayload);
    size_t size;
    auto digest = common::HashString(sPldEscaped, &size);
    auto vDigest = common::ConvertArrayToVector<uint8_t>(digest.get(), size);

    pSig->SetNamedString("alg", "SHA256");
    auto k = pClientPriKey->GetNamedString("d");
    pSig->SetNamedString("dig", RSASignPayload(k, vDigest));

    sPldEscaped = common::ReplaceString(sPldEscaped, "\\\\\\\"pld:\\\\\\\"", "\\\"pld:\\\"");
    sPldEscaped = common::ReplaceString(sPldEscaped, "\\\\\\\"sig:\\\\\\\"", "\\\"sig:\\\"");

    pJsonRoot->SetNamedString("pld", sPldEscaped);
    pJsonRoot->SetNamedObject("sig", *pSig);

    response.serializedLicense = pJsonRoot->Stringify();
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
    shared_ptr<uint8_t> pKey = common::HashString(email, &size);
    auto clc = cache->Lookup(CLCCacheName, CLCCacheTag, pKey.get(), size, true);
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
  std::shared_ptr<std::atomic<bool> >     cancelState)
{
    auto response = PublishResponse();
    auto pJsonRoot = IJsonObject::Create();
    auto pPayload = IJsonObject::Create();

    auto sCLC = GetCLC(authenticationCallback, sEmail, cancelState);
    auto bytearray = common::ByteArray(sCLC.begin(), sCLC.end());

    auto pCLC =  IJsonParser::Create()->Parse(bytearray);
    auto pClcPld = pCLC->GetNamedObject("pld");
    auto pClcPubData = pClcPld->GetNamedObject("pub");
    auto pClcPubPayload = pClcPubData->GetNamedObject("pld");
    auto pClcIssuedTo = pClcPubPayload->GetNamedObject("issto");
    auto pClcIssuer = pClcPubPayload->GetNamedObject("iss");
    auto pServerPubRsaKey = pClcIssuer->GetNamedObject("pubk");

    auto header = IJsonObject::Create();
    header->SetNamedString("ver", "1");
    pPayload->SetNamedObject("hdr", *header);

    response.contentId = common::GenerateAGuid();
    response.owner = pClcIssuedTo->GetNamedString("fname");

    auto license = IJsonObject::Create();
    license->SetNamedString("id", common::GenerateAGuid());
    license->SetNamedString("o", pClcIssuedTo->GetNamedString("fname"));
    license->SetNamedObject("cre", *pClcPubData);

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

    license->SetNamedObject("sad", *pSignedApplicationDataJson);

    auto pEncryptedPolicy = IJsonObject::Create();
    pEncryptedPolicy->SetNamedString("crem", pClcIssuedTo->GetNamedString("em"));
    //encrypted app data would go here
    auto rights = ConvertUserRights(request);
    auto pCustomPolicy = IJsonObject::Create();
    pCustomPolicy->SetNamedArray("usrts", *rights);
    pEncryptedPolicy->SetNamedObject("cp", *pCustomPolicy);

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
    auto esk = RSAEncryptKey(pServerPubRsaKey->GetNamedString("e").c_str(), pServerPubRsaKey->GetNamedString("n").c_str(), sessionkey);

    pSessionKey->SetNamedString("sk", esk);

    license->SetNamedObject("sk", *pSessionKey);

    pPayload->SetNamedObject("lic", *license);

    //sig
    auto pSig = IJsonObject::Create();
    string sPldEscaped = "";//EscapeJson(pPayload);
    size_t size;
    auto digest = common::HashString(sPldEscaped, &size);
    auto vDigest = common::ConvertArrayToVector<uint8_t>(digest.get(), size);
    string sDigest(vDigest.begin(), vDigest.end());
    pSig->SetNamedString("alg", "SHA256");
    pSig->SetNamedString("dig", sDigest);

    pJsonRoot->SetNamedObject("sig", *pSig);

    response.serializedLicense = pJsonRoot->Stringify();
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

//string PublishClient::EscapeJson(shared_ptr<IJsonObject> pPayload)
//{
//    //convert payload to escaped string
//    auto bytePayload = pPayload->Stringify();
//    string sPldBytes = string(bytePayload.begin(), bytePayload.end());
//    string sPldEscaped = "";
//    for (unsigned int i = 0; i < sPldBytes.length(); i++)
//    {
//        string character(1, sPldBytes[i]);
//        sPldEscaped += sPldBytes[i] == '\"' ? "\\\"" : character;
//    }
//    return sPldEscaped;
//}

common::ByteArray PublishClient::EncryptPolicyToBase64(std::shared_ptr<IJsonObject> pPolicy, vector<uint8_t> key, CipherMode cm)
{
    auto crypto = CreateCryptoProvider(cm, key);
    vector<uint8_t> encryptedPolicyBytes = pPolicy->Stringify();

    auto len = encryptedPolicyBytes.size();

    unsigned int blockSize = cm == CIPHER_MODE_CBC4K ? rmscrypto::crypto::CBC4K_BLOCK_SIZE : rmscrypto::crypto::AES128_BLOCK_SIZE;
    auto AlignedSize = len + (len >= blockSize ? (len % blockSize) : (blockSize - len));

    vector<uint8_t> outbuffer(AlignedSize, 0);
    vector<uint8_t> inputbuffer(encryptedPolicyBytes);
    inputbuffer.resize(AlignedSize, 0);

    uint32_t byteswritten = 0;
    crypto->Encrypt(&inputbuffer[0], AlignedSize, 0, false, &outbuffer[0], AlignedSize, &byteswritten);

    return common::ConvertBytesToBase64(outbuffer);
}

string PublishClient::RSAEncryptKey(const char* exp, const char* mod, vector<uint8_t> buf)
{
    RSA* rsa = RSA_new();
    BIGNUM* exponent = BN_new();
    BN_dec2bn(&exponent, exp);
    BIGNUM* modulus_ = BN_new();
    BN_dec2bn(&modulus_, mod);
    rsa->n = modulus_;
    rsa->e = exponent;
    auto size = RSA_size(const_cast<const RSA*>(rsa));
    uint8_t* rsaEncryptedSessionKey = new uint8_t[size];
    int result = RSA_public_encrypt(buf.size(), &buf[0], rsaEncryptedSessionKey, rsa, RSA_PKCS1_PADDING);
    if (result == -1)
        throw exceptions::RMSCryptographyException("Failed to RSA encrypt session key");
    else if (result != size)
        throw exceptions::RMSCryptographyException("RSA_public_encrypt returned unexpected size.");
    vector<uint8_t> vEsk;
    vEsk.assign(rsaEncryptedSessionKey, rsaEncryptedSessionKey + size);
    string sEsk(vEsk.begin(), vEsk.end());

    RSA_free(rsa);
    delete[] rsaEncryptedSessionKey;

    return sEsk;
}
string PublishClient::RSASignPayload(std::string& sPkey, std::vector<uint8_t> digest)
{
    //unfortunately, pkey will be in raw bytes, but openssl needs x509 PEM format
    //so we have to create the x509 cert ourselves

    EVP_PKEY* pkey = EVP_PKEY_new();
    RSA* rsa = RSA_new();
    if (rsa == NULL)
        throw exceptions::RMSCryptographyException("Failed to generate new RSA key.");
    BIGNUM* d = BN_new();
    BN_dec2bn(&d, sPkey.c_str());
    rsa->d = d;
    EVP_PKEY_assign_RSA(pkey, rsa);
    X509* x509 = X509_new();
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);
    X509_set_pubkey(x509, pkey);
    X509_NAME* name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC,
                               (unsigned char *)"US", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC,
                               (unsigned char *)"Microsoft Inc.", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                               (unsigned char *)"localhost", -1, -1, 0);
    X509_set_issuer_name(x509, name);
    auto dig = EVP_sha256();
    X509_sign(x509, pkey, dig);


    auto size = RSA_size(const_cast<const RSA*>(rsa));
    vector<uint8_t> sigbuf(size);
    unsigned int retsize;
    auto sret = RSA_sign(NID_sha256, &digest[0], digest.size(), &sigbuf[0], &retsize, rsa);
    if (sret != 1)
        throw exceptions::RMSCryptographyException("RSA_sign failed. Error: " + string(ERR_error_string(ERR_get_error(), NULL)));
    auto vret = RSA_verify(NID_sha256, &digest[0], digest.size(), &sigbuf[0], retsize, rsa);
    if (vret != 1)
        throw exceptions::RMSCryptographyException("RSA_verify returned false. Error: " + string(ERR_error_string(ERR_get_error(), NULL)));

    X509_free(x509);
    EVP_PKEY_free(pkey); //rsa struct is freed with the evp pkey

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
        for (unsigned int j = 0; j < curr.users.size(); i++)
            _usrs->Append(curr.users[j]);
        for (unsigned int j = 0; j < curr.rights.size(); i++)
            _rghts->Append(curr.rights[j]);
        _user->SetNamedArray("usrs", *_usrs);
        _user->SetNamedArray("rghts", *_rghts);
        userts->Append(*_user);
    }
    return userts;
}

std::string PublishClient::GetCLC(
        modernapi::IAuthenticationCallbackImpl& authenticationCallback,
        const std::string& sEmail,
        std::shared_ptr<std::atomic<bool> > cancelState)
{
    auto pCache = RestClientCache::Create(RestClientCache::CACHE_ENCRYPTED);
    std::string clientcert;
    auto pcacheresult = GetCLCCache(pCache, sEmail);
    if (pcacheresult->CacheMissed) //cache missed, we need to get CLC from server
    {
        /*
        **UNCOMMENT THIS SECTION FOR RELEASE. TEST CODE ONLY**
        auto pRestServiceUrlClient = RestServiceUrlClient::Create();
        auto clcUrl = pRestServiceUrlClient->GetClientLicensorCertificatesUrl(sEmail, authenticationCallback, cancelState);
        auto result = RestHttpClient::Get(clcUrl, authenticationCallback, cancelState);

        if (result.status != StatusCode::OK)
            HandleRestClientError(result.status, result.responseBody);
        */

        //TEST CODE
        std::ifstream ifs;
        ifs.open("/home/rms/Desktop/clc.drm", ifstream::in);
        string str{ istreambuf_iterator<char>(ifs), istreambuf_iterator<char>() };

        auto r = CertificateResponse();
        r.serializedCert = str;

        auto pJsonSerializer = IJsonSerializer::Create();
        try
        {
            //get clc
            auto clc = r;//pJsonSerializer->DeserializeCertificateResponse(result.responseBody);

            clc.serializedCert = Unescape(clc.serializedCert);

            auto bytearray = common::ByteArray(clc.serializedCert.begin(), clc.serializedCert.end());

            auto pCLC =  IJsonParser::Create()->Parse(bytearray);

            auto pri = pCLC->GetNamedObject("pld")->GetNamedObject("pri");

            //cache it
            const common::ByteArray response;
            const std::string exp =  pri->GetNamedString("exp");
            size_t size;
            auto key = common::HashString(sEmail, &size);
            pCache->Store(CLCCacheName, CLCCacheTag, key.get(), size, exp, response, true);
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

string PublishClient::Unescape(string source)
{
    stringstream ss;
    for (size_t i = 0; i < source.length(); i++)
        if (source[i] != '\\')
            ss << source[i];
    auto ret = ss.str();
    ret = common::ReplaceString(ret, "\"{\"", "{\"");
    ret = common::ReplaceString(ret, "}}\"", "}}");
    return ret;
}

shared_ptr<IPublishClient>IPublishClient::Create()
{
  return make_shared<PublishClient>();
}
} // namespace restclients
} // namespace rmscore
