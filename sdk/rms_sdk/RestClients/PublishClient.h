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
#include <functional>
#include <openssl/rsa.h>

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

  std::string RSASignPayload(std::string &sPkey, std::string &sPubkey, std::string &exponent,
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

  std::string GetCLC(const std::string& sEmail, modernapi::IAuthenticationCallbackImpl& authenticationCallback, std::shared_ptr<std::atomic<bool>> cancelState, std::string &outClcPubData);

  common::ByteArray Reformat(common::ByteArray source, int currentlevel = 2);

  common::ByteArray Escape(common::ByteArray source);

  std::shared_ptr<RSA> CreateRSA(BIGNUM* d, BIGNUM* n, BIGNUM *e);

  bool Even(BIGNUM* num);

  PublishResponse PublishCommon(
    common::ByteArray                    && requestBody,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_PUBLISHCLIENT_H_
