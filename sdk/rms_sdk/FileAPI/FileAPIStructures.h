/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/


#ifndef RMS_SDK_FILE_API_FILEAPISTRUCTURES_H
#define RMS_SDK_FILE_API_FILEAPISTRUCTURES_H

#include <string>
#include "ModernAPIExport.h"
#include "IAuthenticationCallback.h"
#include "IConsentCallback.h"
#include "TemplateDescriptor.h"
#include "PolicyDescriptor.h"

namespace rmscore {
namespace fileapi {

enum class UnprotectResult {
  SUCCESS = 0,
  NORIGHTS = 1,
  EXPIRED = 2
};

enum class CryptoOptions {
    AUTO = 0,
    AES256_CBC4K  = 1,
    AES128_ECB = 2
};

struct UserContext
{
    /*!
     * \brief User Id is the email address of the user in context
     */
    std::string userId;

    /*!
     * \brief The callback to be used to get authentication token.
     */
    modernapi::IAuthenticationCallback& authenticationCallback;

    /*!
     * \brief The callback to be used to get consents regarding various options.
     */
    modernapi::IConsentCallback& consentCallback;

    UserContext(const std::string& uId, modernapi::IAuthenticationCallback& authCallback,
                modernapi::IConsentCallback& consntCallback)
        : userId(uId),
          authenticationCallback(authCallback),
          consentCallback(consntCallback) {}
};

struct ProtectWithTemplateOptions
{
    /*!
     * \brief Contains options to specify the chaining algorithm to be used during encryption
     */
    CryptoOptions cryptoOptions;

    /*!
     * \brief The template descriptor object used to create the user policy.
     * Contains template id, name and description
     */
    modernapi::TemplateDescriptor templateDescriptor;

    /*!
     * \brief Singed App data to be stored with this template.
     */
    modernapi::AppDataHashMap signedAppData;

    /*!
     * \brief Content can be opened in a non-RMS-aware app or not.
     */
    bool allowAuditedExtraction;

    ProtectWithTemplateOptions(CryptoOptions options,
                               const modernapi::TemplateDescriptor& descriptor,
                               const modernapi::AppDataHashMap& appData,
                               bool allowExtraction)
        : cryptoOptions(options),
          templateDescriptor(descriptor),
          signedAppData(appData),
          allowAuditedExtraction(allowExtraction) {}
};

struct ProtectWithCustomRightsOptions
{
    /*!
     * \brief Contains options to specify the chaining algorithm to be used during encryption
     */
    CryptoOptions cryptoOptions;

    /*!
     * \brief The policy descriptor which defines the user policy.
     */
    modernapi::PolicyDescriptor policyDescriptor;

    /*!
     * \brief Content can be opened in a non-RMS-aware app or not.
     */
    bool allowAuditedExtraction;

    ProtectWithCustomRightsOptions(CryptoOptions options,
                                   const modernapi::PolicyDescriptor& descriptor,
                                   bool allowExtraction)
        : cryptoOptions(options),
          policyDescriptor(descriptor),
          allowAuditedExtraction(allowExtraction) {}
};

struct UnprotectOptions
{
    /*!
     * \brief If this option is true, the app is not allowed to go to the server for acquiring user policy.
     */
    bool offlineOnly;

    /*!
     * \brief Allows application to cache the acquired policies on disk or memory.
     */
    bool useCache;

    UnprotectOptions() : offlineOnly(false), useCache(true) {}

    UnprotectOptions(bool offline, bool cache)
        : offlineOnly(offline),
          useCache(cache) {}
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_FILEAPISTRUCTURES_H
