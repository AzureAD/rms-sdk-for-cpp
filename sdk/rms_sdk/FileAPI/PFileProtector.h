/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PFILEPROTECTOR_H
#define RMS_SDK_FILE_API_PFILEPROTECTOR_H

#include "BlockBasedProtectedStream.h"
#include "Protector.h"
#include "UserPolicy.h"
#include "PfileHeader.h"

namespace rmscore {
namespace fileapi {

class DLL_PUBLIC_RMS PFileProtector
{
public:
    PFileProtector(const std::string& originalFileExtension);

    ~PFileProtector();

    void ProtectWithTemplate(const std::shared_ptr<std::fstream>& inputStream,
                             const modernapi::TemplateDescriptor& templateDescriptor,
                             const std::string& userId,
                             modernapi::IAuthenticationCallback& authenticationCallback,
                             ProtectionOptions options,
                             const modernapi::AppDataHashMap& signedAppData,
                             const std::shared_ptr<std::fstream>& outputStream,
                             std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    void ProtectWithCustomRights(const std::shared_ptr<std::fstream>& inputStream,
                                 modernapi::PolicyDescriptor& policyDescriptor,
                                 const std::string& userId,
                                 modernapi::IAuthenticationCallback& authenticationCallback,
                                 ProtectionOptions options,
                                 const std::shared_ptr<std::fstream>& outputStream,
                                 std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    UnprotectStatus UnProtect(const std::shared_ptr<std::fstream>& inputStream,
                              const std::string& userId,
                              modernapi::IAuthenticationCallback& authenticationCallBack,
                              modernapi::IConsentCallback& consentCallBack,
                              const bool& isOffline,
                              const std::shared_ptr<std::fstream>& outputStream,
                              std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    bool IsProtected(const std::shared_ptr<std::fstream>& inputStream);

private:

    void Protect(const std::shared_ptr<std::fstream>& inputStream,
                 const std::shared_ptr<std::fstream>& outputStream);

    std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
            const rmscrypto::api::SharedStream& stream,
            const std::shared_ptr<pfile::PfileHeader>& header);

    void EncryptStream(const std::shared_ptr<std::fstream>& stdStream,
                       const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
                       uint64_t originalFileSize);

    void DecryptStream(const std::shared_ptr<std::fstream>& stdStream,
                       const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
                       uint64_t originalFileSize);

    std::shared_ptr<rmscore::pfile::PfileHeader> WriteHeader(
            const rmscrypto::api::SharedStream& stream,
            uint64_t originalFileSize);

    std::shared_ptr<rmscore::pfile::PfileHeader> ReadHeader(
            const rmscrypto::api::SharedStream& stream);

    std::string m_originalFileExtension;
    uint32_t m_blockSize;
    std::shared_ptr<modernapi::UserPolicy> m_userPolicy;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PFILEPROTECTOR_H

