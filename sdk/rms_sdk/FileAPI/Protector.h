/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PROTECTOR_H
#define RMS_SDK_FILE_API_PROTECTOR_H

#include <string>
#include "FileAPIStructures.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace fileapi {

class Protector
{
public:
    DLL_PUBLIC_RMS
    static std::unique_ptr<Protector> Create(const std::string& filePath,
                                             std::shared_ptr<std::fstream> inputStream,
                                             std::string& outputFileName);

    virtual void ProtectWithTemplate(const UserContext& userContext,
                                     const ProtectWithTemplateOptions& options,
                                     std::shared_ptr<std::fstream> outputStream,
                                     std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

    virtual void ProtectWithCustomRights(const UserContext& userContext,
                                         const ProtectWithCustomRightsOptions& options,
                                         std::shared_ptr<std::fstream> outputStream,
                                         std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

    virtual UnprotectResult Unprotect(const UserContext& userContext,
                                      const UnprotectOptions& options,
                                      std::shared_ptr<std::fstream> outputStream,
                                      std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

    virtual bool IsProtected() const = 0;

    virtual ~Protector() { }

protected:
    Protector()
    {}
};

class ProtectorWithWrapper : public Protector
{
public:
    DLL_PUBLIC_RMS
    static std::unique_ptr<ProtectorWithWrapper> Create(const std::string& filePath,
                                             std::shared_ptr<std::fstream> inputStream,
                                             std::string& outputFileName);
    virtual void SetWrapper(std::shared_ptr<std::fstream> inputWrapperStream) = 0;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PROTECTOR_H

