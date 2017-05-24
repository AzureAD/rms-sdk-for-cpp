/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PFILEPROTECTOR_H
#define RMS_SDK_FILE_API_PFILEPROTECTOR_H

#include "IStream.h"
#include "Protector.h"
#include "UserPolicy.h"
#include "PfileHeader.h"

using namespace rmscore::modernapi;
using namespace rmscrypto::api;

namespace rmscore {
namespace fileapi {

class PFileProtector
{
public:
    PFileProtector(std::string originalFileExtension);

    ~PFileProtector();

    void ProtectWithTemplate(std::shared_ptr<std::fstream> inputStream,
                             const TemplateDescriptor& templateDescriptor,
                             const std::string& userId,
                             IAuthenticationCallback& authenticationCallback,
                             const ProtectionOptions& options,
                             const AppDataHashMap& signedAppData,
                             std::shared_ptr<std::fstream> outputStream,
                             std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    void ProtectWithCustomRights(std::shared_ptr<std::fstream> inputStream,
                                 PolicyDescriptor& policyDescriptor,
                                 const std::string& userId,
                                 IAuthenticationCallback& authenticationCallback,
                                 const ProtectionOptions& options,
                                 std::shared_ptr<std::fstream> outputStream,
                                 std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    UnProtectStatus UnProtect(std::shared_ptr<std::fstream> inputStream,
                              const std::string& userId,
                              IAuthenticationCallback& authenticationCallBack,
                              IConsentCallback& consentCallBack,
                              const UnProtectionOptions& options,
                              std::shared_ptr<std::fstream> outputStream,
                              std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    bool IsProtected(std::shared_ptr<std::fstream> inputStream);

private:

    void Protect(std::shared_ptr<std::fstream> inputStream,
                 std::shared_ptr<std::fstream> outputStream);

    std::shared_ptr<rmscore::pfile::PfileHeader> WriteHeader(SharedStream stream,
                                                             uint64_t originalFileSize);

    std::shared_ptr<rmscore::pfile::PfileHeader> ReadHeader(SharedStream stream);

    uint32_t m_blockSize;
    std::shared_ptr<UserPolicy> m_userPolicy;
    std::string m_originalFileExtension;
};

} // namespace officeprotector
} // namespace fileapi

#endif // RMS_SDK_FILE_API_PFILEPROTECTOR_H

