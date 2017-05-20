/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PFILEPROTECTOR_H
#define RMS_SDK_FILE_API_PFILEPROTECTOR_H

#include "IProtector.h"

using namespace rmscore::modernapi;

namespace rmscore {
namespace fileapi {

class DLL_PUBLIC_RMS PFileProtector: public IProtector
{
public:

    PFileProtector(const std::string& originalFileExtension,
               const std::string& newFileExtension);

    ~PFileProtector();

    void ProtectUsingTemplates(std::fstream inputStream,
                                       const TemplateDescriptor& templateDescriptor,
                                       const std::string& userId,
                                       IAuthenticationCallback& authenticationCallback,
                                       UserPolicyCreationOptions options,
                                       const AppDataHashMap& signedAppData,
                                       std::fstream outputStream,
                                       std::shared_ptr<std::atomic<bool> >cancelState) override;

    void ProtectUsingRights(std::fstream inputStream,
                                    const PolicyDescriptor& templateDescriptor,
                                    const std::string& userId,
                                    IAuthenticationCallback& authenticationCallback,
                                    UserPolicyCreationOptions options,
                                    std::fstream outputStream,
                                    std::shared_ptr<std::atomic<bool> >cancelState) override;

    UnProtectStatus UnProtect(std::fstream inputStream,
                                      const std::string& userId,
                                      IAuthenticationCallback& authenticationCallBack,
                                      IConsentCallback* consentCallBack,
                                      PolicyAcquisitionOptions options,
                                      std::fstream outputStream,
                                      ResponseCacheFlags cacheMask = static_cast<ResponseCacheFlags>
            (RESPONSE_CACHE_INMEMORY | RESPONSE_CACHE_ONDISK | RESPONSE_CACHE_CRYPTED),
                                      std::shared_ptr<std::atomic<bool>> cancelState = nullptr) override;

    bool IsProtected(std::fstream inputStream) override;

    std::string GetOriginalFileExtension() override;

    std::string GetNewFileExtension() override;


private:

    uint32_t Read(unsigned char* buffer, uint32_t bufferLen);

    uint32_t Write(const unsigned char* buffer, uint32_t bufferLen);

    std::string m_originalFileExtension;
    std::string newFileExtension;
    uint32_t m_blockSize;
    std::shared_ptr<UserPolicy> m_userPolicy;

};

} // namespace officeprotector
} // namespace fileapi

#endif // RMS_SDK_FILE_API_PFILEPROTECTOR_H

