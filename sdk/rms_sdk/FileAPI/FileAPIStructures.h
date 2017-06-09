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
  Success = 0,
  NoRights = 1,
  Expired = 2
};

enum class CryptoOptions {
    Auto = 0,
    AES256_CBC4K  = 1,
    AES128_ECB = 2
};

struct UserContext
{
    std::string userId;
    modernapi::IAuthenticationCallback& authenticationCallback;
    modernapi::IConsentCallback& consentCallback;

    UserContext(std::string userId, modernapi::IAuthenticationCallback& authenticationCallback,
                modernapi::IConsentCallback& consentCallback)
        : userId(userId),
          authenticationCallback(authenticationCallback),
          consentCallback(consentCallback) {}
};

struct ProtectWithTemplateOptions
{
    CryptoOptions cryptoOptions;
    modernapi::TemplateDescriptor& templateDescriptor;
    modernapi::AppDataHashMap& signedAppData;
    bool allowAuditedExtraction;

    ProtectWithTemplateOptions(CryptoOptions cryptoOptions,
                               modernapi::TemplateDescriptor& templateDescriptor,
                               modernapi::AppDataHashMap& signedAppData,
                               bool allowAuditedExtraction, bool isOffline)
        : cryptoOptions(cryptoOptions),
          templateDescriptor(templateDescriptor),
          signedAppData(signedAppData),
          allowAuditedExtraction(allowAuditedExtraction) {}
};

struct ProtectWithCustomRightsOptions
{
    CryptoOptions cryptoOptions;
    modernapi::PolicyDescriptor& policyDescriptor;
    bool allowAuditedExtraction;

    ProtectWithCustomRightsOptions(CryptoOptions cryptoOptions,
                                   modernapi::PolicyDescriptor& policyDescriptor,
                                   bool allowAuditedExtraction, bool isOffline)
        : cryptoOptions(cryptoOptions),
          policyDescriptor(policyDescriptor),
          allowAuditedExtraction(allowAuditedExtraction) {}
};

struct UnprotectOptions
{
    bool offlineOnly;
    bool useCache;

    UnprotectOptions() : offlineOnly(false), useCache(true) {}

    UnprotectOptions(bool offlineOnly, bool useCache)
        : offlineOnly(offlineOnly),
          useCache(useCache) {}
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_FILEAPISTRUCTURES_H
