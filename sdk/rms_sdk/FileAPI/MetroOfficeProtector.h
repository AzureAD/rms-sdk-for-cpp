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
#include "BlockBasedProtectedStream.h"
#include "FileAPIStructures.h"
#include "UserPolicy.h"
#include "../Pole/pole.h"

namespace rmscore {
namespace fileapi {

class MetroOfficeProtector : public Protector
{
public:
    MetroOfficeProtector(std::shared_ptr<std::fstream> inputStream);

    ~MetroOfficeProtector();

    void ProtectWithTemplate(const UserContext& userContext,
                             const ProtectWithTemplateOptions& options,
                             std::shared_ptr<std::fstream> outputStream,
                             std::shared_ptr<std::atomic<bool>> cancelState);

    void ProtectWithCustomRights(const UserContext& userContext,
                                 const ProtectWithCustomRightsOptions& options,
                                 std::shared_ptr<std::fstream> outputStream,
                                 std::shared_ptr<std::atomic<bool>> cancelState);

    UnprotectResult Unprotect(const UserContext& userContext,
                              const UnprotectOptions& options,
                              std::shared_ptr<std::fstream> outputStream,
                              std::shared_ptr<std::atomic<bool>> cancelState);

    bool IsProtected();

private:
    void Protect(const std::shared_ptr<std::fstream>& outputStream);

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

    modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptions(
            const bool& allowAuditedExtraction,
            CryptoOptions cryptoOptions);

    std::shared_ptr<std::fstream> m_inputStream;
    uint32_t m_blockSize;
    std::shared_ptr<modernapi::UserPolicy> m_userPolicy;
    std::shared_ptr<pole::Storage> m_storage;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H

