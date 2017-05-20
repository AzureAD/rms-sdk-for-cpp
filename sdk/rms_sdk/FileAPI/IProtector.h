/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_IPROTECTOR_H
#define RMS_SDK_FILE_API_IPROTECTOR_H

#include <string>
#include "../Common/CommonTypes.h"
#include "IAuthenticationCallback.h"
#include "IConsentCallback.h"
#include "ModernAPIExport.h"
#include "CacheControl.h"
#include "TemplateDescriptor.h"
#include "PolicyDescriptor.h"
#include "UserPolicy.h"

using namespace rmscore::modernapi;

namespace rmscore {
namespace fileapi {

enum UnProtectStatus {
  Success = 0, NoRights = 1, Expired = 2
};

class DLL_PUBLIC_RMS IProtector
{
public:
    virtual void ProtectWithTemplate(std::fstream inputStream,
                                     const TemplateDescriptor& templateDescriptor,
                                     const std::string& userId,
                                     IAuthenticationCallback& authenticationCallback,
                                     const UserPolicyCreationOptions& options,
                                     const AppDataHashMap& signedAppData,
                                     std::fstream outputStream,
                                     std::shared_ptr<std::atomic<bool>>cancelState = nullptr) = 0;

    virtual void ProtectWithCustomRights(std::fstream inputStream,
                                         const PolicyDescriptor& templateDescriptor,
                                         const std::string& userId,
                                         IAuthenticationCallback& authenticationCallback,
                                         const UserPolicyCreationOptions& options,
                                         std::fstream outputStream,
                                         std::shared_ptr<std::atomic<bool>>cancelState = nullptr) = 0;

    virtual UnProtectStatus UnProtect(std::fstream inputStream,
                                      const std::string& userId,
                                      IAuthenticationCallback& authenticationCallBack,
                                      IConsentCallback* consentCallBack,
                                      PolicyAcquisitionOptions options,
                                      std::fstream outputStream,
                                      ResponseCacheFlags cacheMask = static_cast<ResponseCacheFlags>
            (RESPONSE_CACHE_INMEMORY | RESPONSE_CACHE_ONDISK | RESPONSE_CACHE_CRYPTED),
                                      std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

    virtual bool IsProtected(std::fstream inputStream) = 0;

    virtual std::string GetInputFileExtension() = 0;

    virtual std::string GetOutputFileExtension() = 0;
};

} // namespace officeprotector
} // namespace fileapi

#endif // RMS_SDK_FILE_API_IPROTECTOR_H

