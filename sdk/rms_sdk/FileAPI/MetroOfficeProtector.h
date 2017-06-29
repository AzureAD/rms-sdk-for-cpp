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

class MetroOfficeProtector : public Protector
{
public:
    MetroOfficeProtector(std::string fileName,
                         std::shared_ptr<std::fstream> inputStream);

    ~MetroOfficeProtector();

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
    void ProtectInternal(std::string tempFileName, std::fstream* outputStream);

    UnprotectResult UnprotectInternal(const UserContext& userContext,
                                      const UnprotectOptions& options,
                                      std::shared_ptr<std::fstream> outputStream,
                                      std::string tempFileName,
                                      std::shared_ptr<std::atomic<bool>> cancelState);

    bool IsProtectedInternal(std::string tempFileName) const;


    std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
            const rmscrypto::api::SharedStream& stream,
            uint64_t streamSize,
            std::shared_ptr<rmscrypto::api::ICryptoProvider> cryptoProvider);

    void EncryptStream(const std::shared_ptr<std::fstream> &stdStream,
                       GsfOutput* metroStream,
                       uint64_t originalFileSize);

    void DecryptStream(const std::shared_ptr<std::iostream>& stdStream,
                       GsfInput* metroStream,
                       uint64_t originalFileSize);

    void WriteStreamHeader(GsfOutput* stm,
                           const uint64_t& contentLength);

    void ReadStreamHeader(GsfInput* stm,
                          uint64_t& contentLength);

    modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptions(
            const bool& allowAuditedExtraction,
            CryptoOptions cryptoOptions);

    void CopyFromFileToFstream(FILE* file, std::fstream* stream) const;

    void CopyFromFstreamToFile(FILE* file, std::fstream* stream) const;

    std::string CreateTemporaryFileName() const;

    std::string m_fileName;
    std::shared_ptr<std::fstream> m_inputStream;
    uint32_t m_blockSize;
    std::shared_ptr<modernapi::UserPolicy> m_userPolicy;    
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_METROOFFICEPROTECTOR_H

