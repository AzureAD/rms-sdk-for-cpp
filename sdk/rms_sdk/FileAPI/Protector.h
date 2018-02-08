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

//It is used to fix the compiling warnings.
#define FILEAPI_UNREFERENCED_PARAMETER(p) (void)p

class Protector
{
public:
    DLL_PUBLIC_RMS
    static std::unique_ptr<Protector> Create(const std::string& filePath,
                                             std::shared_ptr<std::fstream> input_stream,
                                             std::string& outputFileName);

    virtual void ProtectWithTemplate(const UserContext& userContext,
                                     const ProtectWithTemplateOptions& options,
                                     std::shared_ptr<std::fstream> output_stream,
                                     std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

    virtual void ProtectWithCustomRights(const UserContext& userContext,
                                         const ProtectWithCustomRightsOptions& options,
                                         std::shared_ptr<std::fstream> output_stream,
                                         std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

    virtual UnprotectResult Unprotect(const UserContext& userContext,
                                      const UnprotectOptions& options,
                                      std::shared_ptr<std::fstream> output_stream,
                                      std::shared_ptr<std::atomic<bool>> cancelState = nullptr) = 0;

    virtual bool IsProtected() const = 0;

    virtual ~Protector() { }

protected:
    Protector()
    {}
};

/**
 * @brief This class is for the type of protector that wraps up the encrypted document
 * with the wrapper document. It extends the ability to set the wrapper document.
 */
class ProtectorWithWrapper : public Protector
{
public:
    /**
     * @brief Creates the protector to protect or unprotect a document.
     * @param[in] filePath          The input file path to be protected. The extension name determines which type of protector to be selected.
     * @param[in] input_stream       The protector will read the original document data from the input file stream.
     * @param[out] outputFileName   It receives the file name of the protected document.
     * @return The protector to protect or unprotect a document.
     */
    DLL_PUBLIC_RMS
    static std::unique_ptr<ProtectorWithWrapper> Create(const std::string& filePath,
                                             std::shared_ptr<std::fstream> input_stream,
                                             std::string& outputFileName);

    /**
     * @brief Sets the wrapper document to wrap up the encrypted document.
     * If the wrapper document is invalid, ProtectWithTemplate will fail and throw an exception.
     * @param[in] inputWrapperStream    The protector will read the wrapper document data from the input file stream.
     * @return void.
     */
    virtual void SetWrapper(std::shared_ptr<std::fstream> inputWrapperStream) = 0;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PROTECTOR_H

