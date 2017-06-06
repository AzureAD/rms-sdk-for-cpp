/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H
#define RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H

#include "BlockBasedProtectedStream.h"
#include "Protector.h"
#include "UserPolicy.h"
#include "../Pole/pole.h"

namespace rmscore {
namespace fileapi {

class DLL_PUBLIC_RMS MetroOfficeProtector
{
public:
    MetroOfficeProtector();

    ~MetroOfficeProtector();

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
            uint64_t streamSize,
            std::shared_ptr<rmscrypto::api::ICryptoProvider> cryptoProvider);

    void EncryptStream(const std::shared_ptr<std::fstream> &stdStream,
                       const std::shared_ptr<pole::Stream>& metroStream,
                       uint64_t originalFileSize);

    void DecryptStream(const std::shared_ptr<std::iostream>& stdStream,
                       const std::shared_ptr<pole::Stream>& metroStream,
                       uint64_t originalFileSize);

    void WriteStreamHeader(const std::shared_ptr<rmscore::pole::Stream>& stm,
                           const uint64_t& contentLength);

    void ReadStreamHeader(const std::shared_ptr<rmscore::pole::Stream>& stm,
                          uint64_t& contentLength);

    uint32_t m_blockSize;
    std::shared_ptr<modernapi::UserPolicy> m_userPolicy;
    std::shared_ptr<pole::Storage> m_storage;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H

