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
    std::string userId;
    modernapi::IAuthenticationCallback& authenticationCallback;
    modernapi::IConsentCallback& consentCallback;

    UserContext(const std::string& uId, modernapi::IAuthenticationCallback& authCallback,
                modernapi::IConsentCallback& consntCallback)
        : userId(uId),
          authenticationCallback(authCallback),
          consentCallback(consntCallback) {}
};

struct ProtectWithTemplateOptions
{
    CryptoOptions cryptoOptions;
    modernapi::TemplateDescriptor templateDescriptor;
    modernapi::AppDataHashMap signedAppData;
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
    CryptoOptions cryptoOptions;
    modernapi::PolicyDescriptor policyDescriptor;
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
    bool offlineOnly;
    bool useCache;

    UnprotectOptions() : offlineOnly(false), useCache(true) {}

    UnprotectOptions(bool offline, bool cache)
        : offlineOnly(offline),
          useCache(cache) {}
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_FILEAPISTRUCTURES_H
