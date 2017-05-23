/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PROTECTOR_H
#define RMS_SDK_FILE_API_PROTECTOR_H

#include <string>
#include "../Common/CommonTypes.h"
#include "IAuthenticationCallback.h"
#include "IConsentCallback.h"
#include "ModernAPIExport.h"
#include "CacheControl.h"
#include "TemplateDescriptor.h"
#include "PolicyDescriptor.h"

using namespace rmscore::modernapi;

namespace rmscore {
namespace fileapi {

enum ProtectorType {
    Pfile = 0,
    Opc = 1,
    Mso = 2,
    Pdf = 3,
    PStar = 4,
};

enum UnProtectStatus {
  Success = 0,
  NoRights = 1,
  Expired = 2
};

enum ProtectionOptions {
  USER_None = 0x0,

  /*!
     @brief Content can be opened in a non-RMS-aware app.
   */
  AllowAuditedExtraction = 0x1,

  /*!
     @brief Deprecated cryptographic algorithms (ECB) are okay. For backwards
        compatibility.
   */
  PreferDeprecatedAlgorithms = 0x2,

  /*!
     @brief Use AES 256 with CBC 4k
   */
  UseAES256_CBC4K = 0x4,
};

enum UnProtectionOptions {
  /*!
     @brief Allow UI and network operations.

     The framework will try to perform the operation silently and offline, but
        will show a UI and connect to a network if necessary.
   */
  Default = 0x0,

  /*!
     @brief Do not allow UI and network operations.

     The framework will try to perform the operation without connecting to a
        network. If it needs to connect to a network, the operation will fail.
        For example, an app can choose not to open a document on the device when
        it is not connected to a WiFi network unless it can be opened offline.
   */
  Offline = 0x1
};

class DLL_PUBLIC_RMS Protector
{
public:
    Protector();

    static void ProtectWithTemplate(const std::string& fileName,
                                    std::fstream inputStream,
                                    const TemplateDescriptor& templateDescriptor,
                                    const std::string& userId,
                                    IAuthenticationCallback& authenticationCallback,
                                    const ProtectionOptions& options,
                                    const AppDataHashMap& signedAppData,
                                    std::fstream outputStream,
                                    std::shared_ptr<std::atomic<bool>>cancelState = nullptr);

    static void ProtectWithCustomRights(const std::string& fileName,
                                        std::fstream inputStream,
                                        const PolicyDescriptor& templateDescriptor,
                                        const std::string& userId,
                                        IAuthenticationCallback& authenticationCallback,
                                        const ProtectionOptions& options,
                                        std::fstream outputStream,
                                        std::shared_ptr<std::atomic<bool>>cancelState = nullptr);

    static UnProtectStatus UnProtect(const std::string& fileName,
                                     std::fstream inputStream,
                                     const std::string& userId,
                                     IAuthenticationCallback* authenticationCallBack,
                                     IConsentCallback* consentCallBack,
                                     UnProtectionOptions options,
                                     std::fstream outputStream,
                                     std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    static bool IsProtected(const std::string& fileName, std::fstream inputStream);

    static std::string GetProtectedFileName(const std::string&fileName);

    static std::string GetUnProtectedFileName(const std::string&fileName);

private:
    static ProtectorType ComputeProtectorType(const std::string& fileExtension, bool isProtect);

    static std::string ComputeNewFileName(const std::string&fileName, bool isProtect);

    static std::string GetFileExtension(const std::string& fileName);
};

} // namespace officeprotector
} // namespace fileapi

#endif // RMS_SDK_FILE_API_PROTECTOR_H

