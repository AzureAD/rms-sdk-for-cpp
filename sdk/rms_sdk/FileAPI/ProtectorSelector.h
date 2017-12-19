/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PROTECTORHELPER_H
#define RMS_SDK_FILE_API_PROTECTORHELPER_H

#include <memory>
#include <map>
#include <string>
#include <vector>
#include "ModernAPIExport.h"

namespace rmscore {
namespace fileapi {

enum class ProtectorType {
    PFILE = 0,
    PSTAR = 1,
    OPC = 2,
    MSO = 3,
    PDF = 4,
};

class IProtectorSelector
{
public:
    DLL_PUBLIC_RMS
    static std::unique_ptr<IProtectorSelector> Create(const std::string& filePath);

    virtual ProtectorType GetProtectorType() = 0;

    virtual std::string GetFileExtension() = 0;

    virtual std::string GetOutputFileName() = 0;

    virtual ~IProtectorSelector(){ }

protected:
    IProtectorSelector()
    {}
};

class ProtectorSelector : public IProtectorSelector
{
public:
    ProtectorSelector(const std::string& filePath);

    virtual ProtectorType GetProtectorType();

    virtual std::string GetFileExtension();

    virtual std::string GetOutputFileName();

private:
    static std::map<std::string, ProtectorType> Init();

    static const std::map<std::string, ProtectorType>& GetProtectorExtensionsMap();

    void Compute(const std::string& fileName);

    std::string m_fileExtension;
    std::string m_newFileName;
    ProtectorType m_pType;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PROTECTORHELPER_H
