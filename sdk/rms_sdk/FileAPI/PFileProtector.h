/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PFILEPROTECTOR_H
#define RMS_SDK_FILE_API_PFILEPROTECTOR_H

#include "Protector.h"
#include "UserPolicy.h"

using namespace rmscore::modernapi;

namespace rmscore {
namespace fileapi {

class PFileProtector
{
public:
    PFileProtector();

    ~PFileProtector();

    void ProtectWithTemplate(std::fstream inputStream,
                             const TemplateDescriptor& templateDescriptor,
                             const std::string& userId,
                             IAuthenticationCallback& authenticationCallback,
                             const ProtectionOptions& options,
                             const AppDataHashMap& signedAppData,
                             std::fstream outputStream,
                             std::shared_ptr<std::atomic<bool> >cancelState);

    void ProtectWithCustomRights(std::fstream inputStream,
                                 const PolicyDescriptor& templateDescriptor,
                                 const std::string& userId,
                                 IAuthenticationCallback& authenticationCallback,
                                 const ProtectionOptions& options,
                                 std::fstream outputStream,
                                 std::shared_ptr<std::atomic<bool> >cancelState);

    UnProtectStatus UnProtect(std::fstream inputStream,
                              const std::string& userId,
                              IAuthenticationCallback* authenticationCallBack,
                              IConsentCallback* consentCallBack,
                              UnProtectionOptions options,
                              std::fstream outputStream,
                              std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    bool IsProtected(std::fstream inputStream);

private:
    uint32_t Read(unsigned char* buffer, uint32_t bufferLen);

    uint32_t Write(const unsigned char* buffer, uint32_t bufferLen);

    uint32_t m_blockSize;
    std::shared_ptr<UserPolicy> m_userPolicy;
};

} // namespace officeprotector
} // namespace fileapi

#endif // RMS_SDK_FILE_API_PFILEPROTECTOR_H

