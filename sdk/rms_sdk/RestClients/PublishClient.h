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
#include "../../rmscrypto_sdk/CryptoAPI/CryptoAPI.h"

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
    std::shared_ptr<std::atomic<bool> >     cancelState)
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
    std::shared_ptr<std::atomic<bool> >     cancelState)
  override;

private:
  const size_t KEY_SIZE = 256;
  const size_t AES_BLOCK_SIZE = 16;

  std::string Unescape(std::string source);

  std::string RSASignPayload(std::string &sPkey,
    std::vector<uint8_t> digest);

  std::string RSAEncryptKey(const common::ByteArray &exp,
    const common::ByteArray &mod,
    std::vector<uint8_t> buf);

  common::ByteArray EncryptPolicyToBase64(
    std::shared_ptr<platform::json::IJsonObject> pPolicy,
    std::vector<uint8_t>                         key,
    rmscrypto::api::CipherMode                   cm);

  std::shared_ptr<platform::json::IJsonArray> ConvertUserRights(
     const PublishCustomRequest& request);

  std::shared_ptr<CLCCacheResult> GetCLCCache(
    std::shared_ptr<IRestClientCache> cache,
    const std::string               & email);

  std::string GetCLC(const std::string& sEmail, std::string &outClcPubData);

  std::string Reformat(std::string source);

  PublishResponse PublishCommon(
    common::ByteArray                    && requestBody,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_PUBLISHCLIENT_H_
