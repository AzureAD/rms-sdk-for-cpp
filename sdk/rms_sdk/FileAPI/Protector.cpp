/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "Protector.h"
#include <fstream>
#include "MetroOfficeProtector.h"
#include "PFileProtector.h"
#include "RMSExceptions.h"
#include "UserPolicy.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

Protector::Protector(std::string fileName, std::shared_ptr<std::fstream> inputStream)
{
    if(fileName.empty() || !inputStream->is_open())
    {
        throw exceptions::RMSInvalidArgumentException("Invalid arguments provided");
    }
    m_filename = fileName;
    m_inputStream = inputStream;
    m_protectorSelector = std::make_shared<ProtectorSelector>(fileName);
}

void Protector::ProtectWithTemplate(UserContext& userContext,
                                    const ProtectWithTemplateOptions& options,
                                    std::shared_ptr<std::fstream> outputStream,
                                    std::shared_ptr<std::atomic<bool>> cancelState)
{
    if(options.m_isOffline)
    {
        throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                            "Offline publishing is not supported");
    }
    ProtectorType pType = m_protectorSelector->GetProtectorType();

    switch (pType)
    {
        case ProtectorType::Opc:
        {
            auto upOptions = options.m_allowAuditedExtraction ?
                        modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
                        modernapi::UserPolicyCreationOptions::USER_None;
            if(options.m_algorithm == ProtectionAlgorithm::Default ||
                    options.m_algorithm == ProtectionAlgorithm::ECB )
            {
                upOptions = static_cast<modernapi::UserPolicyCreationOptions>(
                            upOptions | modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
            }
            else    //temporary until we have CBC for office files
            {
                throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                    "CBC Encryption with Office files is not yet"
                                                    "supported");
            }
            auto metroProtector = std::make_shared<MetroOfficeProtector>();
            metroProtector->ProtectWithTemplate(m_inputStream, options.m_templateDescriptor,
                                                userContext.m_userId, userContext.m_authenticationCallback,
                                                upOptions, options.m_signedAppData, outputStream,
                                                cancelState);
        }
        break;

        case ProtectorType::Pfile:
        {
            auto upOptions = options.m_allowAuditedExtraction ?
                        modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
                        modernapi::UserPolicyCreationOptions::USER_None;
            if(options.m_algorithm == ProtectionAlgorithm::ECB )
            {
                upOptions = static_cast<modernapi::UserPolicyCreationOptions>(
                            upOptions | modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
            }
            auto pfileProtector = std::make_shared<PFileProtector>(m_protectorSelector->GetFileExtension());
            pfileProtector->ProtectWithTemplate(m_inputStream, options.m_templateDescriptor,
                                                userContext.m_userId, userContext.m_authenticationCallback,
                                                upOptions, options.m_signedAppData, outputStream,
                                                cancelState);
        }
        break;

        default:
        {
            throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                "");
        }
        break;
    }
}

void Protector::ProtectWithCustomRights(UserContext& userContext,
                                        const ProtectWithCustomRightsOptions& options,
                                        std::shared_ptr<std::fstream> outputStream,
                                        std::shared_ptr<std::atomic<bool>> cancelState)
{
    if(options.m_isOffline)
    {
        throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                            "Offline publishing is not supported");
    }
    ProtectorType pType = m_protectorSelector->GetProtectorType();

    switch (pType)
    {
        case ProtectorType::Opc:
        {
            auto upOptions = options.m_allowAuditedExtraction ?
                        modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
                        modernapi::UserPolicyCreationOptions::USER_None;
            if(options.m_algorithm == ProtectionAlgorithm::Default ||
                    options.m_algorithm == ProtectionAlgorithm::ECB )
            {
                upOptions = static_cast<modernapi::UserPolicyCreationOptions>(
                            upOptions | modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
            }
            else    //temporary until we have CBC for office files
            {
                throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                    "CBC Encryption with Office files is not yet"
                                                    "supported");
            }
            auto metroProtector = std::make_shared<MetroOfficeProtector>();
            metroProtector->ProtectWithCustomRights(m_inputStream, options.m_policyDescriptor,
                                                    userContext.m_userId,
                                                    userContext.m_authenticationCallback, upOptions,
                                                    outputStream, cancelState);
        }
        break;

        case ProtectorType::Pfile:
        {
            auto upOptions = options.m_allowAuditedExtraction ?
                        modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
                        modernapi::UserPolicyCreationOptions::USER_None;
            if(options.m_algorithm == ProtectionAlgorithm::ECB )
            {
                upOptions = static_cast<modernapi::UserPolicyCreationOptions>(
                            upOptions | modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
            }
            auto pfileProtector = std::make_shared<PFileProtector>(m_protectorSelector->GetFileExtension());
            pfileProtector->ProtectWithCustomRights(m_inputStream, options.m_policyDescriptor,
                                                    userContext.m_userId,
                                                    userContext.m_authenticationCallback, upOptions,
                                                    outputStream, cancelState);
        }
        break;

        default:
        {
            throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                "");
        }
        break;
    }
}

UnprotectStatus Protector::Unprotect(UserContext& userContext,
                                     const UnprotectOptions& options,
                                     std::shared_ptr<std::fstream> outputStream,
                                     std::shared_ptr<std::atomic<bool>> cancelState)
{
    ProtectorType pType = m_protectorSelector->GetProtectorType();

    switch (pType)
    {
        case ProtectorType::Opc:
        {
            auto metroProtector = std::make_shared<MetroOfficeProtector>();
            metroProtector->Unprotect(m_inputStream, userContext.m_userId,
                                      userContext.m_authenticationCallback, userContext.m_consentCallback,
                                      options.m_isOffline, options.m_useCache, outputStream, cancelState);
        }
        break;

        case ProtectorType::Pfile:
        {
            auto pfileProtector = std::make_shared<PFileProtector>(m_protectorSelector->GetFileExtension());
            pfileProtector->Unprotect(m_inputStream, userContext.m_userId,
                                      userContext.m_authenticationCallback, userContext.m_consentCallback,
                                      options.m_isOffline, options.m_useCache, outputStream, cancelState);
        }
        break;

        default:
        {
            throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                "File Type not supported");
        }
        break;
    }
}

bool Protector::IsProtected()
{
    ProtectorType pType = m_protectorSelector->GetProtectorType();

    switch (pType)
    {
        case ProtectorType::Opc:
        {
            auto metroProtector = std::make_shared<MetroOfficeProtector>();
            metroProtector->IsProtected(m_inputStream);
        }
        break;

        case ProtectorType::Pfile:
        {
            auto pfileProtector = std::make_shared<PFileProtector>(m_protectorSelector->GetFileExtension());
            pfileProtector->IsProtected(m_inputStream);
        }
        break;

        default:
        {
            throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                "File Type not supported");
        }
        break;
    }
}

std::string Protector::GetOutputFileName()
{
    return m_protectorSelector->GetOutputFileName();
}

} // namespace fileapi
} // namespace rmscore
