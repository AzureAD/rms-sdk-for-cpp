/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

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
#include "QUuid"

using namespace std;
using namespace rmscore::json;
using namespace rmscore::platform::http;
using namespace rmscore::platform::json;

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
    auto pJsonAll = IJsonObject::Create();

    auto sCLC = GetCLC(authenticationCallback, sEmail, cancelState);
    auto bytearray = common::ByteArray(sCLC.begin(), sCLC.end());

    auto pCLC =  IJsonParser::Create()->Parse(bytearray);
    auto pClcPubData = pCLC->GetNamedObject("pub");
    auto pClcPubPayload = pClcPubData->GetNamedObject("pub");
    auto pClcIssuedTo = pClcPubPayload->GetNamedObject("issto");
    auto pClcIssuer = pClcPubPayload->GetNamedObject("iss");

    auto header = IJsonObject::Create();
    header->SetNamedString("ver", "1");
    pJsonAll->SetNamedObject("hdr", *header);

    auto license = IJsonObject::Create();
    license->SetNamedString("id", QUuid::createUuid().toString().toStdString());
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
    //session key
    license->SetNamedObject("enp", *pEncryptedPolicy);
    //sig


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

std::string& PublishClient::GetCLC(
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
            auto issto = pCLC->GetNamedObject("pub")->GetNamedObject("pld");
            auto pubkey = issto->GetNamedObject("pubk");
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

shared_ptr<IPublishClient>IPublishClient::Create()
{
  return make_shared<PublishClient>();
}
} // namespace restclients
} // namespace rmscore
