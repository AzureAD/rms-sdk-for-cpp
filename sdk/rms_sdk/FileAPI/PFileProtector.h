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
#include "BlockBasedProtectedStream.h"
#include "FileAPIStructures.h"
#include "UserPolicy.h"
#include "PfileHeader.h"

namespace rmscore {
namespace fileapi {

class PFileProtector : public Protector
{
public:
    PFileProtector(const std::string& originalFileExtension,
                   std::shared_ptr<std::fstream> inputStream);

    ~PFileProtector();

    void ProtectWithTemplate(const UserContext& userContext,
                             const ProtectWithTemplateOptions& options,
                             std::shared_ptr<std::fstream> outputStream,
                             std::shared_ptr<std::atomic<bool>> cancelState) override;

    void ProtectWithCustomRights(const UserContext& userContext,
                                 const ProtectWithCustomRightsOptions& options,
                                 std::shared_ptr<std::fstream> outputStream,
                                 std::shared_ptr<std::atomic<bool>> cancelState) override;

    UnprotectResult Unprotect(const UserContext& userContext,
                              const UnprotectOptions& options,
                              std::shared_ptr<std::fstream> outputStream,
                              std::shared_ptr<std::atomic<bool>> cancelState) override;

    bool IsProtected() const override;

private:
    void Protect(const std::shared_ptr<std::fstream>& outputStream);

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
            const rmscrypto::api::SharedStream& stream) const;

    modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptions(
            const bool& allowAuditedExtraction,
            CryptoOptions cryptoOptions);

    std::string original_file_extension_;
    std::shared_ptr<std::fstream> input_stream_;
    uint32_t block_size_;
    std::shared_ptr<modernapi::UserPolicy> user_policy_;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PFILEPROTECTOR_H

