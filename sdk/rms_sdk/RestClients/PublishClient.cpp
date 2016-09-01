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
    return LocalPublishCommon(false, (void*)(&request), sizeof request, authenticationCallback, sEmail, cancelState, getCLCCallback);
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
    PublishCustomRequest pcr(false, false);
    PublishUsingTemplateRequest putr;
    if (isAdhoc)
    {
        if (cbRequest != sizeof pcr)
            throw exceptions::RMSInvalidArgumentException("Request size did not match given size.");
        pcr = *((PublishCustomRequest*)_request);
    }
    else
    {
        if (cbRequest != sizeof putr)
            throw exceptions::RMSInvalidArgumentException("Request size did not match given size.");
        putr = *((PublishUsingTemplateRequest*)_request);
    }
    auto sad = isAdhoc ? pcr.signedApplicationData : putr.signedApplicationData;
    auto pda = isAdhoc ? pcr.bPreferDeprecatedAlgorithms : putr.bPreferDeprecatedAlgorithms;

    auto pPayload = IJsonObject::Create();

    string clcPubData;
    auto pCLC = CreateCLC(clcPubData, getCLCCallback, sEmail, authenticationCallback, cancelState);
    auto pCLCPld = pCLC->GetNamedObject("pld");

    auto em = pCLCPld->GetNamedObject("pub")->GetNamedObject("pld")->GetNamedObject("issto")->GetNamedString("em");

    RSAInit(pCLC);

    SetHeader(pPayload);

    auto license = CreateLicense(pCLCPld, clcPubData);

    if (sad.size() > 0)
        license->SetNamedObject("sad", *(CreateSignedAppData(sad)));

    CipherMode cm = pda ? CIPHER_MODE_ECB : CIPHER_MODE_CBC4K;

    vector<uint8_t> sessionkey;
    auto contentkey = SetSessionKey(license, pda, sessionkey);

    //encrypted policy
    shared_ptr<IJsonObject> pEncryptedPolicy;
    if (isAdhoc)
        pEncryptedPolicy = CreatePolicyAdhoc(pcr, em);
    else
        pEncryptedPolicy = CreatePolicyTemplate(putr, em);
    license->SetNamedValue("enp", EncryptBytesToBase64(pEncryptedPolicy->Stringify(), sessionkey, cm));

    pPayload->SetNamedObject("lic", *license);

    auto vFinal = SignPayload(pPayload->Stringify());

    return CreateResponse(vFinal, sad, cm, contentkey, em);
}

PublishResponse PublishClient::PublishCommon(
  vector<uint8_t>                    && requestBody,
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


shared_ptr<IJsonObject> PublishClient::CreateCLC(
  string &outClcPubData,
  const std::function<std::string(std::string, std::string&)>& getCLCCallback,
  string sEmail,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  shared_ptr<atomic<bool>> cancelState)
{
    string sCLC;
    if (getCLCCallback == nullptr)
        sCLC = GetCLC(sEmail, authenticationCallback, cancelState, outClcPubData);
    else
        sCLC = getCLCCallback(sEmail, outClcPubData);
    auto ByteArray = vector<uint8_t>(sCLC.begin(), sCLC.end());
    auto pCLC =  IJsonParser::Create()->Parse(ByteArray);
    return pCLC;
}

PublishResponse PublishClient::CreateResponse(std::vector<uint8_t> licenseNoBOM, modernapi::AppDataHashMap signedAppData, CipherMode cm, std::vector<uint8_t> contentkey, string ownerName)
{
    auto response = PublishResponse();
    //add byte-order marker
    vector<uint8_t> UTF8bom = { 0xEF, 0xBB, 0xBF };
    licenseNoBOM.insert(licenseNoBOM.begin(), UTF8bom.begin(), UTF8bom.end());
    response.serializedLicense = licenseNoBOM;

    auto responseKey = KeyDetailsResponse();
    responseKey.algorithm = "AES";
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
    auto pClcPubData = clcPayload->GetNamedObject("pub");
    auto pClcPubPayload = pClcPubData->GetNamedObject("pld");
    auto pClcIssuedTo = pClcPubPayload->GetNamedObject("issto");

    auto license = IJsonObject::Create();
    license->SetNamedString("id", common::GenerateAGuid());
    license->SetNamedString("o", pClcIssuedTo->GetNamedString("fname"));
    license->SetNamedString("cre", clcPubData);
    license->SetNamedString("cid", pClcIssuedTo->GetNamedString("em"));

    auto clcPrivData = clcPayload->GetNamedObject("pri");
    license->SetNamedString("lcd", clcPrivData->GetNamedString("issd"));
    license->SetNamedString("exp", clcPrivData->GetNamedString("exp"));
    return license;
}

shared_ptr<IJsonObject> PublishClient::CreatePolicyAdhoc(PublishCustomRequest request, string isstoEmail)
{
    auto pEncryptedPolicy = IJsonObject::Create();
    pEncryptedPolicy->SetNamedString("crem", isstoEmail);
    //encrypted app data would go here
    auto rights = ConvertUserRights(request);
    auto pCustomPolicy = IJsonObject::Create();
    pCustomPolicy->SetNamedArray("usrts", *rights);
    pEncryptedPolicy->SetNamedObject("cp", *pCustomPolicy);
    return pEncryptedPolicy;
}

shared_ptr<IJsonObject> PublishClient::CreatePolicyTemplate(PublishUsingTemplateRequest request, string isstoEmail)
{
    auto pEncryptedPolicy = IJsonObject::Create();
    pEncryptedPolicy->SetNamedString("crem", isstoEmail);
    pEncryptedPolicy->SetNamedString("tid", request.templateId);
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
    auto pubk = pClc->GetNamedObject("pld")->GetNamedObject("pub")->GetNamedObject("pld")->GetNamedObject("issto")->GetNamedObject("pubk");
    auto d = common::ConvertBase64ToBytes(pClc->GetNamedObject("pld")->GetNamedObject("pri")->GetNamedObject("prik")->GetNamedValue("d"));
    auto n = common::ConvertBase64ToBytes(pubk->GetNamedValue("n"));
    auto e = pubk->GetNamedValue("e");

    rsaKeyBlob = ICryptoEngine::Create()->CreateRSAKeyBlob(d, e, n, true);
}

vector<uint8_t> PublishClient::SetSessionKey(shared_ptr<IJsonObject> pLicense, bool prefDeprecatedAlgs, vector<uint8_t>& outSK)
{
    auto pSessionKey = IJsonObject::Create();

    //session key init
    const unsigned int keysize = AES_BLOCK_SIZE;
    vector<uint8_t> sessionkey(keysize);
    RAND_bytes(&sessionkey[0], keysize);

    vector<uint8_t> contentkey(keysize);
    RAND_bytes(&contentkey[0], keysize);

    auto pContentKey = IJsonObject::Create();
    pContentKey->SetNamedString("alg", "AES");
    pContentKey->SetNamedString("cm", "CBC4K");
    pContentKey->SetNamedValue("k", EncryptBytesToBase64(contentkey, sessionkey, prefDeprecatedAlgs ? CIPHER_MODE_ECB : CIPHER_MODE_CBC4K));
    auto bytes = common::ConvertBytesToBase64(pContentKey->Stringify());
    pSessionKey->SetNamedObject("eck", *pContentKey);

    pSessionKey->SetNamedValue("eck", bytes);
    pSessionKey->SetNamedString("alg", "AES");
    pSessionKey->SetNamedString("cm", prefDeprecatedAlgs ? "ECB" : "CBC4K");

    auto esk = common::ConvertBytesToBase64(rsaKeyBlob->PublicEncrypt(sessionkey));
    pSessionKey->SetNamedString("sk", string(esk.begin(), esk.end()));

    pLicense->SetNamedObject("sk", *pSessionKey);

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

    uint32_t retsize;
    auto _signed = rsaKeyBlob->Sign(digest, retsize);
    string errmsg;
    if (!rsaKeyBlob->VerifySignature(_signed, digest, errmsg, retsize))
        throw exceptions::RMSCryptographyException("Could not verify payload signature: " + errmsg);

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

vector<uint8_t> PublishClient::EncryptBytesToBase64(vector<uint8_t> bytesToEncrypt, vector<uint8_t> key, CipherMode cm)
{
    auto crypto = CreateCryptoProvider(cm, key);

    auto totalLen = bytesToEncrypt.size();

    const bool CBC4K = cm == CIPHER_MODE_CBC4K;

    unsigned int blockSize = CBC4K ? rmscrypto::crypto::CBC4K_BLOCK_SIZE : rmscrypto::crypto::AES128_BLOCK_SIZE;
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
//        auto pRestServiceUrlClient = RestServiceUrlClient::Create();
//        auto clcUrl = pRestServiceUrlClient->GetClientLicensorCertificatesUrl(sEmail, authenticationCallback, cancelState);
//        auto request = IJsonObject::Create();
//        request->SetNamedString("SignatureEncoding", "utf-8");
//        auto result = RestHttpClient::Post(clcUrl, request->Stringify(),authenticationCallback, cancelState);

//        if (result.status != StatusCode::OK)
//            HandleRestClientError(result.status, result.responseBody);

          /*TEST CODE*/
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
