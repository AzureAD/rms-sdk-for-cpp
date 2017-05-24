/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H
#define RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H

#include "Protector.h"
#include "UserPolicy.h"
#include "pole.h"

using namespace rmscore::modernapi;
using namespace rmscore:: pole;

namespace rmscore {
namespace fileapi {

class MetroOfficeProtector
{
public:
    MetroOfficeProtector();

    ~MetroOfficeProtector();

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

    uint32_t ReadStreamHeader(uint64_t& contentLength);

    uint32_t WriteStreamHeader(const uint64_t& contentLength);

    uint32_t m_blockSize;
    std::shared_ptr<UserPolicy> m_userPolicy;
    std::shared_ptr<Storage> m_storage;
};

} // namespace officeprotector
} // namespace fileapi

#endif // RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H

