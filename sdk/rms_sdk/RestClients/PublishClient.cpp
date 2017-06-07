/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <openssl/rand.h>
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
#include "../Common/Constants.h"
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

PublishResponse PublishClient::LocalPublishUsingTemplate(
  const PublishUsingTemplateRequest     & request,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const std::string                       sEmail,
  std::shared_ptr<std::atomic<bool> >     cancelState,
  const std::function<std::string(std::string, std::string&)>& getCLCCallback)
{
    //return PublishResponse();
    return LocalPublishCommon(false, (void*)(&request), sizeof request, authenticationCallback, sEmail, cancelState, getCLCCallback);
}

PublishResponse PublishClient::LocalPublishCustom(
  const PublishCustomRequest            & request,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const std::string                       sEmail,
  std::shared_ptr<std::atomic<bool> >     cancelState,
  const std::function<std::string(std::string, std::string&)>& getCLCCallback)
{
    return LocalPublishCommon(true, (void*)(&request), sizeof request, authenticationCallback, sEmail, cancelState, getCLCCallback);
}


PublishResponse PublishClient::LocalPublishCommon(bool isAdhoc,
  void *_request,
  size_t cbRequest,
  modernapi::IAuthenticationCallbackImpl &authenticationCallback,
  const string &sEmail,
  std::shared_ptr<std::atomic<bool> > cancelState,
  const std::function<string(string, string &)> &getCLCCallback)
{

    //determines which request to use
    PublishUsingTemplateRequest publishUsingTemplateRequest;


    PublishCustomRequest publishCustomRequest(false, false);

    if (isAdhoc)
    {
        if (cbRequest != sizeof publishCustomRequest)
            throw exceptions::RMSInvalidArgumentException("Request size did not match given size.");
        publishCustomRequest = *((PublishCustomRequest*)_request);
    }
    else
    {
        if (cbRequest != sizeof publishUsingTemplateRequest)
            throw exceptions::RMSInvalidArgumentException("Request size did not match given size.");
        publishUsingTemplateRequest = *((PublishUsingTemplateRequest*)_request);
    }

    auto signedAppData = isAdhoc ? publishCustomRequest.signedApplicationData : publishUsingTemplateRequest.signedApplicationData;
    auto preferDeprecatedAlgs = isAdhoc ? publishCustomRequest.bPreferDeprecatedAlgorithms : publishUsingTemplateRequest.bPreferDeprecatedAlgorithms;

    auto pPayload = IJsonObject::Create();

    string clcPubData;
    shared_ptr<IJsonObject> pCLC;

    try
    {
        //check if we entirely failed to get a CLC.
        //this should only occur for onprem use cases.
        //if it does occur, we will use the old online publishing flow.
        pCLC = CreateCLC(clcPubData, getCLCCallback, sEmail, authenticationCallback, cancelState);
    }
    catch (exceptions::RMSEndpointNotFoundException)
    {
        return isAdhoc ? PublishCustom(publishCustomRequest, authenticationCallback, sEmail, cancelState) :
                         PublishUsingTemplate(publishUsingTemplateRequest, authenticationCallback, sEmail, cancelState);
    }
    auto pCLCPld = pCLC->GetNamedObject(JsonConstants::PAYLOAD);

    auto em = pCLCPld->GetNamedObject(JsonConstants::PUBLIC_DATA)->GetNamedObject(JsonConstants::PAYLOAD)->GetNamedObject(JsonConstants::ISSUED_TO)->GetNamedString(JsonConstants::EMAIL);

    RSAInit(pCLC);

    SetHeader(pPayload);

    auto license = CreateLicense(pCLCPld, clcPubData);


    if (signedAppData.size() > 0)
        license->SetNamedObject(JsonConstants::SIGNED_APPLICATION_DATA, *(CreateSignedAppData(signedAppData)));

    CipherMode cm = preferDeprecatedAlgs ? CIPHER_MODE_ECB : CIPHER_MODE_CBC4K;
    vector<uint8_t> sessionkey;
    auto contentkey = SetSessionKey(license, preferDeprecatedAlgs, sessionkey);

    //encrypted policy
    shared_ptr<IJsonObject> pEncryptedPolicy;
    if (isAdhoc)
        pEncryptedPolicy = CreatePolicyAdhoc(publishCustomRequest, em);
    else
        pEncryptedPolicy = CreatePolicyTemplate(publishUsingTemplateRequest, em);
    license->SetNamedValue(JsonConstants::ENCRYPTED_POLICY, EncryptBytesToBase64(pEncryptedPolicy->Stringify(), sessionkey, cm));
    pPayload->SetNamedObject(JsonConstants::LICENSE, *license);

   auto vFinal = SignPayload(pPayload->Stringify());

    auto result = CreateResponse(vFinal, signedAppData, cm, contentkey, em);
    return result;
}

shared_ptr<CLCCacheResult> PublishClient::GetCLCCache(shared_ptr<IRestClientCache> cache, const std::string& email)
{
    shared_ptr<CLCCacheResult> result;
    size_t size;
    vector<uint8_t> pKey = common::HashString(vector<uint8_t>(email.begin(), email.end()), &size);
    auto clc = cache->Lookup(CLCCacheName, CLCCacheTag, &pKey[0], size, true);
    return clc.capacity() > 0 ? make_shared<CLCCacheResult>(clc.at(0), false) : make_shared<CLCCacheResult>("", true);
}


shared_ptr<IJsonObject> PublishClient::CreateCLC(
  string &outClcPubData,
  const std::function<std::string(std::string, std::string&)>& getCLCCallback,
  const string sEmail,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  shared_ptr<atomic<bool>> cancelState)
{
    string sCLC;
    if(getCLCCallback == nullptr){
        sCLC = RetrieveCLC(sEmail, authenticationCallback, cancelState, outClcPubData);
    }else{
        sCLC = getCLCCallback(sEmail, outClcPubData);
    }
    auto result = IJsonParser::Create()->Parse(vector<uint8_t>(sCLC.begin(), sCLC.end()));
    return result;
}

PublishResponse PublishClient::CreateResponse(std::vector<uint8_t> licenseNoBOM, modernapi::AppDataHashMap signedAppData, CipherMode cm, std::vector<uint8_t> contentkey, string ownerName)
{
    auto response = PublishResponse();
    //add byte-order marker
    vector<uint8_t> UTF8bom = { 0xEF, 0xBB, 0xBF };
    licenseNoBOM.insert(licenseNoBOM.begin(), UTF8bom.begin(), UTF8bom.end());
    response.serializedLicense = licenseNoBOM;

    auto responseKey = KeyDetailsResponse();
    responseKey.algorithm = rmscrypto::crypto::szAES;
    responseKey.cipherMode = ICryptoProvider::CipherModeString(cm);
    responseKey.value = common::ConvertBytesToBase64(contentkey);
    response.key = responseKey;

    response.contentId = common::GenerateAGuid();
    response.owner = ownerName;
    response.signedApplicationData = signedAppData;
    response.name = "Restricted Access";
    response.description = "Permission is currently restricted. Only specified users can access this content.";

    return response;
}

shared_ptr<IJsonObject> PublishClient::CreateLicense(std::shared_ptr<IJsonObject> clcPayload, string clcPubData)
{
    auto pClcPubData = clcPayload->GetNamedObject(JsonConstants::PUBLIC_DATA);
    auto pClcPubPayload = pClcPubData->GetNamedObject(JsonConstants::PAYLOAD);
    auto pClcIssuedTo = pClcPubPayload->GetNamedObject(JsonConstants::ISSUED_TO);

    auto license = IJsonObject::Create();
    license->SetNamedString(JsonConstants::ID, common::GenerateAGuid());
    license->SetNamedString(JsonConstants::OWNER, pClcIssuedTo->GetNamedString(JsonConstants::FRIENDLY_NAME));
    license->SetNamedString(JsonConstants::CREATOR, clcPubData);
    license->SetNamedString(JsonConstants::CONTENT_ID, pClcIssuedTo->GetNamedString(JsonConstants::EMAIL));

    auto clcPrivData = clcPayload->GetNamedObject(JsonConstants::PRIVATE_DATA);
    license->SetNamedString(JsonConstants::LICENSE_CREATION_DATE, clcPrivData->GetNamedString(JsonConstants::ISSUED_TIME));
    license->SetNamedString(JsonConstants::EXPIRATION_TIME, clcPrivData->GetNamedString(JsonConstants::EXPIRATION_TIME));
    return license;
}

shared_ptr<IJsonObject> PublishClient::CreatePolicyAdhoc(PublishCustomRequest request, string isstoEmail)
{
    auto pEncryptedPolicy = IJsonObject::Create();
    pEncryptedPolicy->SetNamedString(JsonConstants::CREATOR_EMAIL, isstoEmail);
    //encrypted app data would go here
    auto rights = ConvertUserRights(request);
    auto pCustomPolicy = IJsonObject::Create();
    pCustomPolicy->SetNamedArray(JsonConstants::USER_RIGHTS, *rights);
    pEncryptedPolicy->SetNamedObject(JsonConstants::CUSTOM_POLICY, *pCustomPolicy);
    return pEncryptedPolicy;
}

shared_ptr<IJsonObject> PublishClient::CreatePolicyTemplate(PublishUsingTemplateRequest request, string isstoEmail)
{
    auto pEncryptedPolicy = IJsonObject::Create();
    pEncryptedPolicy->SetNamedString(JsonConstants::CREATOR_EMAIL, isstoEmail);
    pEncryptedPolicy->SetNamedString(JsonConstants::TEMPLATE_ID, request.templateId);
    return pEncryptedPolicy;
}

shared_ptr<IJsonObject> PublishClient::CreateSignedAppData(modernapi::AppDataHashMap appdata)
{
    auto pSignedApplicationDataJson = IJsonObject::Create();
    for_each(
      begin(appdata),
      end(appdata),
      [ = ](const pair<string, string>& appData)
        {
          pSignedApplicationDataJson->SetNamedString(appData.first,
                                                     appData.second);
        });
    return pSignedApplicationDataJson;
}

void PublishClient::SetHeader(std::shared_ptr<IJsonObject> pld)
{
    auto header = IJsonObject::Create();
    header->SetNamedString("ver", "1.0.0.0");
    pld->SetNamedObject("hdr", *header);
}

void PublishClient::RSAInit(shared_ptr<IJsonObject> pClc)
{
    auto pubk = pClc->
            GetNamedObject(JsonConstants::PAYLOAD)->
            GetNamedObject(JsonConstants::PUBLIC_DATA)->
            GetNamedObject(JsonConstants::PAYLOAD)->
            GetNamedObject(JsonConstants::ISSUED_TO)->
            GetNamedObject(JsonConstants::PUBLIC_KEY);

    auto d = common::ConvertBase64ToBytes(
                pClc->GetNamedObject(JsonConstants::PAYLOAD)->
                GetNamedObject(JsonConstants::PRIVATE_DATA)->
                GetNamedObject(JsonConstants::PRIVATE_KEY)->
                GetNamedValue(JsonConstants::PRIVATE_MODULUS));

    auto n = common::ConvertBase64ToBytes(pubk->GetNamedValue(JsonConstants::PUBLIC_MODULUS));

    auto e = pubk->GetNamedValue(JsonConstants::PUBLIC_EXPONENT);

    rsaKeyBlob = CreateCryptoEngine()->CreateRSAKeyBlob(d, e, n, true); //set to false to skip key verification check
}

vector<uint8_t> PublishClient::SetSessionKey(shared_ptr<IJsonObject> pLicense, bool prefDeprecatedAlgs, vector<uint8_t>& outSK)
{
    auto pSessionKey = IJsonObject::Create();

    //session key init
    const unsigned int keysize = AES_BLOCK_SIZE_BYTES;
    vector<uint8_t> sessionkey(keysize);
    RAND_bytes(&sessionkey[0], keysize);

    vector<uint8_t> contentkey(keysize);
    RAND_bytes(&contentkey[0], keysize);

    auto pContentKey = IJsonObject::Create();
    pContentKey->SetNamedString(JsonConstants::ALGORITHM, rmscrypto::crypto::szAES);
    pContentKey->SetNamedString(JsonConstants::CIPHER_MODE, rmscrypto::crypto::szCBC4K);
    pContentKey->SetNamedValue(JsonConstants::KEY, EncryptBytesToBase64(contentkey, sessionkey, prefDeprecatedAlgs ? CIPHER_MODE_ECB : CIPHER_MODE_CBC4K));
    auto bytes = common::ConvertBytesToBase64(pContentKey->Stringify());
    pSessionKey->SetNamedObject(JsonConstants::ENCRYPTED_CONTENT_KEY, *pContentKey);

    pSessionKey->SetNamedValue(JsonConstants::ENCRYPTED_CONTENT_KEY, bytes);
    pSessionKey->SetNamedString(JsonConstants::ALGORITHM, rmscrypto::crypto::szAES);
    pSessionKey->SetNamedString(JsonConstants::CIPHER_MODE, prefDeprecatedAlgs ? rmscrypto::crypto::szECB : rmscrypto::crypto::szCBC4K);

    auto esk = common::ConvertBytesToBase64(rsaKeyBlob->PublicEncrypt(sessionkey));
    pSessionKey->SetNamedString(JsonConstants::SESSION_KEY, string(esk.begin(), esk.end()));

    pLicense->SetNamedObject(JsonConstants::SESSION_KEY, *pSessionKey);

    outSK = sessionkey;

    return contentkey;
}

vector<uint8_t> PublishClient::SignPayload(std::vector<uint8_t> pld)
{
    size_t size;
    auto toHash = Reformat(pld, 1);
    string sToHash(toHash.begin(), toHash.end());
    auto digest = common::HashString(toHash, &size, false);
    toHash = Escape(toHash);
    sToHash = string(toHash.begin(), toHash.end());

    uint32_t retSize;
    auto _signed = rsaKeyBlob->Sign(digest, retSize);
    string errmsg;
    if (!rsaKeyBlob->VerifySignature(_signed, digest, errmsg, retSize)){
        throw exceptions::RMSCryptographyException("Could not verify payload signature: " + errmsg);
    }

    auto b64 = common::ConvertBytesToBase64(_signed);

    string sSig = R"("sig":{"alg":"SHA1", "penc":"utf-8", "dig":")" + string(b64.begin(), b64.end()) + "\"}}";
    string sFinal = R"({"pld":")" + sToHash + R"(",)" + sSig;

    return vector<uint8_t>(sFinal.begin(), sFinal.end());
}

vector<uint8_t> PublishClient::Escape(vector<uint8_t> source)
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
    return vector<uint8_t>(ret.begin(), ret.end());
}

vector<uint8_t> PublishClient::Reformat(vector<uint8_t> source, int currentlevel)
{
    //TODO: Get rid of this
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
                if (s != "\\\\")
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
        auto pos = ret.find(R"(\\\"sig\\\")");
        if (pos == std::string::npos){
            throw exceptions::RMSInvalidArgumentException("source");
        }
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

vector<uint8_t> PublishClient::EncryptBytesToBase64(vector<uint8_t> bytesToEncrypt, vector<uint8_t> key, CipherMode cm)
{
    auto crypto = CreateCryptoProvider(cm, key);

    auto totalLen = bytesToEncrypt.size();

    const bool isCBC4K = cm == CIPHER_MODE_CBC4K;

    unsigned int blockSize = isCBC4K ? rmscrypto::crypto::CBC4K_BLOCK_SIZE : rmscrypto::crypto::AES128_BLOCK_SIZE;
    vector<vector<uint8_t>> blockList((totalLen / blockSize) + 1);

    for (unsigned int i = 0; i < blockList.size(); i++)
    {
        bool isFinal = i == blockList.size() - 1;
        if (!isFinal)
            blockList[i] = vector<uint8_t>(bytesToEncrypt.begin() + (i * blockSize), bytesToEncrypt.begin() + ((i + 1) * blockSize));
        else
        {
            vector<uint8_t> finalBlock(bytesToEncrypt.begin() + (i * blockSize), bytesToEncrypt.end());
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
        if (isCBC4K)
        {
            if (!isLastBlock)
                outbuffer = vector<uint8_t>(blockSize, 0);
            else
                outbuffer = vector<uint8_t>(PADDED_CBC4K_SIZE, 0);
        }
        crypto->Encrypt(&inputbuffer[0], blockSize, i, isLastBlock, &outbuffer[0], outbuffer.size(), &byteswritten);
        uint32_t actualEncryptedBytes = _size - (_size % AES_KEY_SIZE) + AES_KEY_SIZE;
        final.insert(
          final.end(),
          outbuffer.begin(),
          outbuffer.begin() + (_size < rmscrypto::crypto::CBC4K_BLOCK_SIZE ? actualEncryptedBytes : PADDED_CBC4K_SIZE));
        len -= blockSize;
    }
    return common::ConvertBytesToBase64(final);
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

std::string PublishClient::RetrieveCLC(const std::string& sEmail, modernapi::IAuthenticationCallbackImpl& authenticationCallback, std::shared_ptr<std::atomic<bool>> cancelState, string& outClcPubData)
{
    auto pCache = RestClientCache::Create(RestClientCache::CACHE_ENCRYPTED);
    std::string clientcert;
    auto pcacheresult = GetCLCCache(pCache, sEmail);
    pcacheresult->CacheMissed = true;
    if (pcacheresult->CacheMissed) //cache missed, we need to get CLC from server
    {
        auto pRestServiceUrlClient = RestServiceUrlClient::Create();
        auto clcUrl = pRestServiceUrlClient->GetClientLicensorCertificatesUrl(sEmail, authenticationCallback, cancelState);
        if (clcUrl == "")
        {
            throw exceptions::RMSEndpointNotFoundException("Could not find ClientLicensorCertificates URL.");
        }
        auto request = IJsonObject::Create();
        request->SetNamedString("SignatureEncoding", "utf-8");
        auto result = RestHttpClient::Post(clcUrl, request->Stringify(),authenticationCallback, cancelState);

        if (result.status != StatusCode::OK)
            HandleRestClientError(result.status, result.responseBody);

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

            auto ByteArray = vector<uint8_t>(clc.serializedCert.begin(), clc.serializedCert.end());

            auto pCLC =  IJsonParser::Create()->Parse(ByteArray);

            auto pri = pCLC->GetNamedObject("pld")->GetNamedObject("pri");

            //cache it
            const vector<uint8_t> response;
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
