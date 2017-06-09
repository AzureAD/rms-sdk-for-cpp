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
#include "ProtectorSelector.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace fileapi {

class DLL_PUBLIC_RMS Protector
{
public:

    Protector(std::string fileName, std::shared_ptr<std::fstream> inputStream);

    void ProtectWithTemplate(UserContext& userContext,
                             const ProtectWithTemplateOptions& options,
                             std::shared_ptr<std::fstream> outputStream,
                             std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    void ProtectWithCustomRights(UserContext& userContext,
                                 const ProtectWithCustomRightsOptions& options,
                                 std::shared_ptr<std::fstream> outputStream,
                                 std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    UnprotectStatus Unprotect(UserContext& userContext,
                              const UnprotectOptions& options,
                              std::shared_ptr<std::fstream> outputStream,
                              std::shared_ptr<std::atomic<bool>> cancelState = nullptr);

    bool IsProtected();

    std::string GetOutputFileName();

private:

    std::string m_filename;
    std::shared_ptr<std::fstream> m_inputStream;
    std::shared_ptr<ProtectorSelector> m_protectorSelector;

};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PROTECTOR_H

