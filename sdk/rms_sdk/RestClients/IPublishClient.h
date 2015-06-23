/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_IPUBLISHCLIENT_H_
#define _RMS_LIB_IPUBLISHCLIENT_H_

#include "../ModernAPI/IAuthenticationCallbackImpl.h"
#include "RestObjects.h"

namespace rmscore {
namespace restclients {
class IPublishClient {
public:

  virtual PublishResponse PublishUsingTemplate(
    const PublishUsingTemplateRequest     & request,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                       email)
    = 0;
  virtual PublishResponse PublishCustom(
    const PublishCustomRequest            & request,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                       email)
    = 0;

public:

  static std::shared_ptr<IPublishClient>Create();
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_IPUBLISHCLIENT_H_
