/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H
#define RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H

//#define _GLIBCXX_USE_CXX11_ABI 0

#include <cstdio>
#include "Protector.h"
#include "BlockBasedProtectedStream.h"
#include "FileAPIStructures.h"
#include "UserPolicy.h"
#include <gsf/gsf.h>

namespace rmscore {
namespace fileapi {

static const uint64_t MAX_FILE_SIZE_FOR_ENCRYPT = 1ul << 30;
static const uint64_t MAX_FILE_SIZE_FOR_DECRYPT = 3ul << 30;
static const uint64_t BUF_SIZE = 4096;

class MetroOfficeProtector : public Protector
{
public:
    MetroOfficeProtector(std::string fileName,
                         std::shared_ptr<std::istream> inputStream);

    ~MetroOfficeProtector();

    void ProtectWithTemplate(const UserContext& userContext,
                             const ProtectWithTemplateOptions& options,
                             std::shared_ptr<std::ostream> outputStream,
                             std::shared_ptr<std::atomic<bool>> cancelState) override;

    void ProtectWithCustomRights(const UserContext& userContext,
                                 const ProtectWithCustomRightsOptions& options,
                                 std::shared_ptr<std::ostream> outputStream,
                                 std::shared_ptr<std::atomic<bool>> cancelState) override;

    UnprotectResult Unprotect(const UserContext& userContext,
                              const UnprotectOptions& options,
                              std::shared_ptr<std::ostream> outputStream,
                              std::shared_ptr<std::atomic<bool>> cancelState) override;

    bool IsProtected() const override;

private:
    void ProtectInternal(FILE* tempFile, std::string tempFileName, uint64_t inputFileSize);

    UnprotectResult UnprotectInternal(const UserContext& userContext,
                                      const UnprotectOptions& options,
                                      std::shared_ptr<std::ostream> outputStream,
                                      std::string tempFileName,
                                      uint64_t inputFileSize,
                                      std::shared_ptr<std::atomic<bool>> cancelState);

    bool IsProtectedInternal(std::string tempFileName, uint64_t inputFileSize) const;


    std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
            const rmscrypto::api::SharedStream& stream,
            uint64_t streamSize,
            std::shared_ptr<rmscrypto::api::ICryptoProvider> cryptoProvider);

    void EncryptStream(GsfOutput* metroStream,
                       uint64_t originalFileSize);

    void DecryptStream(const std::shared_ptr<std::ostream>& stdStream,
                       GsfInput* metroStream,
                       uint64_t originalFileSize);

    void WriteStreamHeader(GsfOutput* stm,
                           const uint64_t& contentLength);

    void ReadStreamHeader(GsfInput* stm,
                          uint64_t& contentLength);

    modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptions(
            const bool& allowAuditedExtraction,
            CryptoOptions cryptoOptions);

    void CopyFromFileToOstream(FILE* file, std::ostream* stream) const;

    void CopyFromIstreamToFile(std::string tempFileName, uint64_t inputFileSize) const;

    std::string CreateTemporaryFileName() const;

    uint64_t GetFileSize(uint64_t maxFileSize) const;

    std::string m_fileName;
    std::shared_ptr<std::istream> m_inputStream;
    uint32_t m_blockSize;
    std::shared_ptr<modernapi::UserPolicy> m_userPolicy;    
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H

