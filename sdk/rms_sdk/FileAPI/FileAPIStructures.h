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

enum UnprotectStatus {
  Success = 0,
  NoRights = 1,
  Expired = 2
};

enum ProtectionAlgorithm {
    Default = 0,
    CBC = 1,
    ECB = 2
};

struct DLL_PUBLIC_RMS UserContext
{
    std::string m_userId;
    modernapi::IAuthenticationCallback& m_authenticationCallback;
    modernapi::IConsentCallback& m_consentCallback;

    UserContext(std::string userId, modernapi::IAuthenticationCallback& authenticationCallback,
                modernapi::IConsentCallback& consentCallback) : m_userId(userId),
                m_authenticationCallback(authenticationCallback),
                m_consentCallback(consentCallback) {}
};

struct DLL_PUBLIC_RMS ProtectWithTemplateOptions
{
    ProtectionAlgorithm m_algorithm;
    modernapi::TemplateDescriptor& m_templateDescriptor;
    modernapi::AppDataHashMap& m_signedAppData;
    bool m_allowAuditedExtraction;
    bool m_isOffline; //not supported as of now

    ProtectWithTemplateOptions(ProtectionAlgorithm algorithm,
                               modernapi::TemplateDescriptor& templateDescriptor,
                               modernapi::AppDataHashMap& signedAppData,
                               bool allowAuditedExtraction, bool isOffline):
        m_algorithm(algorithm), m_templateDescriptor(templateDescriptor), m_signedAppData(signedAppData),
        m_allowAuditedExtraction(allowAuditedExtraction), m_isOffline(isOffline) {}
};

struct DLL_PUBLIC_RMS ProtectWithCustomRightsOptions
{
    ProtectionAlgorithm m_algorithm;
    modernapi::PolicyDescriptor& m_policyDescriptor;
    bool m_allowAuditedExtraction;
    bool m_isOffline; //not supported as of now

    ProtectWithCustomRightsOptions(ProtectionAlgorithm algorithm,
                                   modernapi::PolicyDescriptor& policyDescriptor,
                                   bool allowAuditedExtraction, bool isOffline):
        m_algorithm(algorithm), m_policyDescriptor(policyDescriptor),
        m_allowAuditedExtraction(allowAuditedExtraction), m_isOffline(isOffline) {}
};

struct DLL_PUBLIC_RMS UnprotectOptions
{
    bool m_isOffline;
    bool m_useCache;

    UnprotectOptions(bool isOffline, bool useCache): m_isOffline(isOffline), m_useCache(useCache) {}
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_FILEAPISTRUCTURES_H
