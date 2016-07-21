/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "openssl/rand.h"
#include "openssl/rsa.h"
#include "PublishClient.h"
#include "RestClientCache.h"
#include "RestClientErrorHandling.h"
#include "RestHttpClient.h"
#include "RestServiceUrls.h"
#include "RestServiceUrlClient.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Json/IJsonSerializer.h"
#include "../Platform/Json/IJsonObject.h"
#include "../Platform/Json/IJsonParser.h"
#include "../Platform/Json/JsonObjectQt.h"
#include "../Common/tools.h"

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
    auto pPayload = IJsonObject::Create();

    auto sCLC = GetCLC(authenticationCallback, sEmail, cancelState);
    auto bytearray = common::ByteArray(sCLC.begin(), sCLC.end());

    auto pCLC =  IJsonParser::Create()->Parse(bytearray);
    auto pClcPubData = pCLC->GetNamedObject("pub");
    auto pClcPubPayload = pClcPubData->GetNamedObject("pub");
    auto pClcIssuedTo = pClcPubPayload->GetNamedObject("issto");
    auto pClcIssuer = pClcPubPayload->GetNamedObject("iss");
    auto pServerPubRsaKey = pClcIssuer->GetNamedObject("pubk");

    auto header = IJsonObject::Create();
    header->SetNamedString("ver", "1");
    pPayload->SetNamedObject("hdr", *header);

    auto license = IJsonObject::Create();
    license->SetNamedString("id", common::GenerateAGuid());
    license->SetNamedString("o", pClcIssuedTo->GetNamedString("fname"));
    license->SetNamedObject("cre", *pClcPubData);

    auto clcPrivData = pCLC->GetNamedObject("pri");
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
    //encrypted app data

    unique_ptr<uint8_t[]> skbuf(new uint8_t(KEY_SIZE));
    RAND_bytes(skbuf.get(), KEY_SIZE);
    CipherMode cm = request.bPreferDeprecatedAlgorithms ? CIPHER_MODE_ECB : CIPHER_MODE_CBC4K; //dont know if I need to be doing this, i think it's always going to be ECB
    auto crypto = CreateCryptoProvider(cm, common::ConvertArrayToVector<uint8_t>(skbuf.get()));
    vector<uint8_t> encryptedPolicyBytes = pEncryptedPolicy->Stringify();
    auto encPolByteArr = &encryptedPolicyBytes[0];
    auto len = sizeof encryptedPolicyBytes;
    auto AlignedSize = len + (len % AES_BLOCK_SIZE);
    unique_ptr<uint8_t[]> outbuffer(new uint8_t[AlignedSize]);
    unique_ptr<uint8_t[]> inputbuffer(new uint8_t[AlignedSize]);
    memset(input, 0, AlignedSize);
    memcpy(inputbuffer, encPolByteArr, AlignedSize);
    uint32_t byteswritten = 0;
    crypto->Encrypt(inputbuffer.get(), AlignedSize, 0, false, outputbufer.get(), AlignedSize, &byteswritten);
    if (cm == CIPHER_MODE_ECB)
        if (byteswritten != AlignedSize)
            throw exceptions::RMSCryptographyException("Wrote an invalid number of bytes.");
    else
        if (byteswritten != len)
            throw exceptions::RMSCryptographyException("Wrote an invalid number of bytes.");
    license->SetNamedValue("enp", common::ConvertBytesToBase64(common::ConvertArrayToVector<uint8_t>(outputbuffer.get())));

    //session key
    auto pSessionKey = IJsonObject::Create();
    unique_ptr<uint8_t[]> buf(new uint8_t[KEY_SIZE]);
    RAND_bytes(buf.get(), KEY_SIZE);
    auto pContentKey = IJsonObject::Create();
    pContentKey->SetNamedString("alg", "AES");
    pContentKey->SetNamedString("cm", "CBC4K");
    pContentKey->SetNamedValue("k", common::ConvertArrayToVector<uint8_t>(buf.get()));
    auto bytes = common::ConvertBytesToBase64(pContentKey->Stringify());
    pSessionKey->SetNamedString("eck", std::string(bytes.begin(), bytes.end()));
    pSessionKey->SetNamedString("alg", "AES");
    pSessionKey->SetNamedString("cm", request.bPreferDeprecatedAlgorithms ? "ECB" : "CBC4K");
    //encrypt session key with public RSA key from CLC
    shared_ptr<RSA> rsa = RSA_new();
    BIGNUM* exponent = BN_new();
    BN_dec2bn(&exponent, pServerPubRsaKey->GetNamedString("e").c_str());
    BIGNUM* modulus_ = BN_new();
    BN_dec2bn(&modulus_, pServerPubRsaKey->GetNamedString("n").c_str());
    rsa->n = modulus_;
    rsa->e = exponent;
    unique_ptr<uint8_t[]> rsaEncryptedSessionKey(new uint8_t[RSA_size(rsa)]);
    int result = RSA_public_encrypt(KEY_SIZE, skbuf.get(), rsaEncryptedSessionKey, rsa, RSA_PKCS1_PADDING);
    if (result != 1)
        throw exceptions::RMSCryptographyException("Failed to RSA encrypt session key");

    pPayload->SetNamedObject("lic", license);
    //sig
    auto pSig = IJsonObject::Create();

    return PublishResponse();
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
  auto pJsonSerializer   = IJsonSerializer::Create();
  auto serializedRequest =
    pJsonSerializer->SerializePublishCustomRequest(request);

  return PublishResponse();
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
        auto pRestServiceUrlClient = RestServiceUrlClient::Create();
        auto clcUrl = pRestServiceUrlClient->GetClientLicensorCertificatesUrl(sEmail, authenticationCallback, cancelState);
        auto result = RestHttpClient::Get(clcUrl, authenticationCallback, cancelState);

        if (result.status != StatusCode::OK)
            HandleRestClientError(result.status, result.responseBody);

        auto pJsonSerializer = IJsonSerializer::Create();
        try
        {
            //get clc
            auto clc = pJsonSerializer->DeserializeCertificateResponse(result.responseBody);
            auto bytearray = common::ByteArray(clc.serializedCert.begin(), clc.serializedCert.end());

            auto pCLC =  IJsonParser::Create()->Parse(bytearray);
            auto pub = pCLC->GetNamedObject("pub")->GetNamedObject("pld");
            auto pri = pCLC->GetNamedObject("pri");

            //cache it
            const common::ByteArray response;
            const std::string exp =  pri->GetNamedString("exp");
            size_t size;
            auto key = common::HashString(sEmail, &size);
            pCache->Store(CLCCacheName, CLCCacheTag, key.get(), size, exp, response, true);
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
//std::shared_ptr<ICryptoKey> PublishClient::CreateKey(size_t size, CryptoAlgorithm algorithm, shared_ptr<const uint8_t[]>& outBuf)
//{
//    outBuf = make_shared(new uint8_t[size]);
//    RAND_bytes(const_cast<const uint8_t[]>(outBuf.get()), size);
//    auto engine = ICryptoEngine::Create();
//    return engine->CreateKey(outBuf, size, algorithm);
//}

shared_ptr<IPublishClient>IPublishClient::Create()
{
  return make_shared<PublishClient>();
}
} // namespace restclients
} // namespace rmscore
