/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_TEMPLATESCLIENT_H_
#define _RMS_LIB_TEMPLATESCLIENT_H_

#include "ITemplatesClient.h"
namespace rmscore {
namespace restclients {
class TemplatesClient : public ITemplatesClient {
public:

  virtual TemplateListResponse GetTemplates(
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & sEmail,
    std::shared_ptr<std::atomic<bool> >     cancelState) override;
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_TEMPLATESCLIENT_H_
