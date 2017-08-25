/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_OFFICE_PROTECTOR_METROOFFICEPROTECTOR_H
#define RMS_SDK_OFFICE_PROTECTOR_METROOFFICEPROTECTOR_H

#include <cstdio>
#include "Protector.h"
#include "BlockBasedProtectedStream.h"
#include "FileAPIStructures.h"
#include "UserPolicy.h"
#include "../PFile/Utils.h"
#include <gsf/gsf.h>

using namespace rmscore::fileapi;

namespace rmscore {
namespace officeprotector {

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

    /*!
     * \brief ProtectInternal
     *
     *
     * \param outputTempFileName
     * \param inputFileSize
     */
    void ProtectInternal(std::string outputTempFileName, uint64_t inputFileSize);

    UnprotectResult UnprotectInternal(const UserContext& userContext,
                                      const UnprotectOptions& options,
                                      std::shared_ptr<std::ostream> outputStream,
                                      std::string inputTempFileName,
                                      uint64_t inputFileSize,
                                      std::shared_ptr<std::atomic<bool>> cancelState);

    bool IsProtectedInternal(std::string inputTempFileName, uint64_t inputFileSize) const;


    std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
            const rmscrypto::api::SharedStream& stream,
            uint64_t streamSize,
            std::shared_ptr<rmscrypto::api::ICryptoProvider> cryptoProvider);

    void EncryptStream(GsfOutput* metroStream,
                       uint64_t inputFileSize);

    void DecryptStream(const std::shared_ptr<std::ostream>& stdStream,
                       GsfInput* metroStream,
                       uint64_t originalFileSize);

    std::string m_fileName;
    std::shared_ptr<std::istream> m_inputStream;
    uint32_t m_blockSize;
    std::shared_ptr<modernapi::UserPolicy> m_userPolicy;
};

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_METROOFFICEPROTECTOR_H

