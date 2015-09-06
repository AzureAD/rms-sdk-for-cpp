/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_ITEMPLATESCLIENT_H_
#define _RMS_LIB_ITEMPLATESCLIENT_H_

#include <memory>
#include <atomic>

#include "RestObjects.h"
#include "../ModernAPI/IAuthenticationCallbackImpl.h"
namespace rmscore {
namespace restclients {
class ITemplatesClient {
public:

  virtual TemplateListResponse GetTemplates(
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState) = 0;

public:

  static std::shared_ptr<ITemplatesClient>Create();
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_ITEMPLATESCLIENT_H_
