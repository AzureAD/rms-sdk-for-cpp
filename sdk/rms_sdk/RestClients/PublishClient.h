/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_PUBLISHCLIENT_H_
#define _RMS_LIB_PUBLISHCLIENT_H_
#include "IPublishClient.h"
#include "CLCCacheResult.h"
#include "RestClientCache.h"
#include "../Platform/Json/IJsonObject.h"
#include "../Platform/Json/IJsonArray.h"
#include "../Platform/Json/IJsonParser.h"
#include "../Platform/Json/JsonObjectQt.h"
#include "../../rmscrypto_sdk/CryptoAPI/CryptoAPI.h"
#include <functional>
#include <unordered_map>

using namespace rmscore::platform::json;
namespace rmscore {
namespace restclients {
class PublishClient : public IPublishClient {
public:
  virtual PublishResponse PublishUsingTemplate(
    const PublishUsingTemplateRequest     & request,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                       sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState)
  override;
  virtual PublishResponse LocalPublishUsingTemplate(
    const PublishUsingTemplateRequest     & request,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                       sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState,
   const std::function<std::string(std::string, std::string&)>& getCLCCallback = nullptr)
  override;
  virtual PublishResponse PublishCustom(
    const PublishCustomRequest            & request,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                       sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState)
  override;
  virtual PublishResponse LocalPublishCustom(
    const PublishCustomRequest            & request,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                       sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState,
    const std::function<std::string(std::string, std::string&)>& getCLCCallback = nullptr)
  override;

private:
  const size_t KEY_SIZE = 256;
  const size_t AES_BLOCK_SIZE = 16;

  std::string Unescape(std::string source, bool skipReformat = false);

  vector<uint8_t> EncryptBytesToBase64(std::vector<uint8_t>                         bytesToEncrypt,
    std::vector<uint8_t>                         key,
    rmscrypto::api::CipherMode                   cm);

  std::shared_ptr<platform::json::IJsonArray> ConvertUserRights(
     const PublishCustomRequest& request);

  std::shared_ptr<CLCCacheResult> GetCLCCache(
    std::shared_ptr<IRestClientCache> cache,
    const std::string               & email);

  std::string GetCLC(const std::string& sEmail, modernapi::IAuthenticationCallbackImpl& authenticationCallback, std::shared_ptr<std::atomic<bool>> cancelState, std::string &outClcPubData);

  std::vector<uint8_t> Reformat(std::vector<uint8_t> source, int currentlevel = 2);

  std::vector<uint8_t> Escape(std::vector<uint8_t> source);

  std::shared_ptr<rmscrypto::api::IRSAKeyBlob> rsaKeyBlob;

  std::shared_ptr<IJsonObject> CreateCLC(std::string& outClcPubData, const std::function<std::string(std::string, std::string&)>& getCLCCallback, std::string sEmail, modernapi::IAuthenticationCallbackImpl& authenticationCallback, std::shared_ptr<std::atomic<bool>> cancelState);

  std::shared_ptr<IJsonObject> CreateLicense(std::shared_ptr<IJsonObject> clcPayload, std::string clcPubData);

  std::shared_ptr<IJsonObject> CreatePolicyAdhoc(PublishCustomRequest request, std::string isstoEmail);

  std::shared_ptr<IJsonObject> CreatePolicyTemplate(PublishUsingTemplateRequest request, std::string isstoEmail);

  std::shared_ptr<IJsonObject> CreateSignedAppData(modernapi::AppDataHashMap appdata);

  PublishResponse CreateResponse(std::vector<uint8_t> licenseNoBOM, modernapi::AppDataHashMap signedAppData, rmscrypto::api::CipherMode cm, std::vector<uint8_t> contentkey, string ownerName);

  void RSAInit(std::shared_ptr<IJsonObject> pClc);

  vector<uint8_t> SetSessionKey(std::shared_ptr<IJsonObject> pLicense, bool prefDeprecatedAlgs, vector<uint8_t> &outSK);

  void SetHeader(std::shared_ptr<IJsonObject> pld);

  std::vector<uint8_t> SignPayload(std::vector<uint8_t> pld);

  PublishResponse LocalPublishCommon(bool                                    isAdhoc,
    void*                                   request, size_t cbRequest,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState,
    const std::function<std::string(std::string, std::string&)>& getCLCCallback);

  PublishResponse PublishCommon(
    vector<uint8_t>                    && requestBody,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_PUBLISHCLIENT_H_
