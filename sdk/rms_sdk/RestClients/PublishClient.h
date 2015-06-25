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
namespace rmscore {
namespace restclients {
class PublishClient : public IPublishClient {
public:

  virtual PublishResponse PublishUsingTemplate(
    const PublishUsingTemplateRequest     & request,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                       sEmail)
  override;
  virtual PublishResponse PublishCustom(
    const PublishCustomRequest            & request,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                       sEmail)
  override;

private:

  PublishResponse PublishCommon(
    common::ByteArray                    && requestBody,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & sEmail);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_PUBLISHCLIENT_H_
