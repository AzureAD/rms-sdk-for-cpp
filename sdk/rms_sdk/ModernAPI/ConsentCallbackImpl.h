/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_CONSENTCALLBACKIMPL_H_
#define _RMS_LIB_CONSENTCALLBACKIMPL_H_

#include <string>
#include "IConsentCallbackImpl.h"
#include "IConsentCallback.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
class ConsentCallbackImpl : public IConsentCallbackImpl {
public:

  ConsentCallbackImpl(IConsentCallback  *callback,
                      const std::string& userId,
                      bool               isPublishing);
  virtual void Consents(const std::string             & email,
                        const std::string             & domain,
                        const std::vector<std::string>& urls) override;

protected:

  IConsentCallback *m_callback;
  std::shared_ptr<std::string> m_userId;
  bool                         m_isPublishing;
  std::shared_ptr<std::string> m_domain;
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_CONSENTCALLBACKIMPL_H_
