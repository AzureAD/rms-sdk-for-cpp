/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_TEMPLATEDESCRIPTOR_H_
#define _RMS_LIB_TEMPLATEDESCRIPTOR_H_

#include <future>
#include "IAuthenticationCallback.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace core {
class ProtectionPolicy;
}
namespace modernapi {
/// <summary>
/// Template information
/// </summary>
class DLL_PUBLIC_RMS TemplateDescriptor {
public:

  /// <summary>
  /// Template Id
  /// </summary>
  std::string TemplateId() const {
    return m_id;
  }

  /// <summary>
  /// Policy name
  /// </summary>
  std::string Name() const {
    return m_name;
  }

  /// <summary>
  /// Policy description
  /// </summary>
  std::string Description() const {
    return m_description;
  }

  /**
   * @brief Get list of templates for current tenant.
   * @param userId The email address of the user for whom the templates
   *  are being retrieved. This email address will be used to discover the RMS
   *  service instance (either ADRMS server or Azure RMS) that the user's
   *  organization is using. This parameter is also used as a hint for userId
   *  for user authentication, i.e., it will be passed to
   *  IAuthenticationCallback.GetToken() in the AuthenticationParameters
   * structure.
   * @param authenticationCallback Callback to utilize for auth.
   * @return the list of templates
   */
  static std::shared_future<std::shared_ptr<std::vector<TemplateDescriptor> > >
  GetTemplateListAsync(
    const std::string                & userId,
    IAuthenticationCallback          & authenticationCallback,
    std::launch                        launchType,
    std::shared_ptr<std::atomic<bool> >cancelState = nullptr);


  TemplateDescriptor(const std::string& id,
                     const std::string& name,
                     const std::string& description);
  TemplateDescriptor(std::shared_ptr<core::ProtectionPolicy>policy);

private:

  std::string m_id;
  std::string m_name;
  std::string m_description;
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_TEMPLATEDESCRIPTOR_H_
