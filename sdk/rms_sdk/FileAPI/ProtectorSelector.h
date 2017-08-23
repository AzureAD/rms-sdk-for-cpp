/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PROTECTORHELPER_H
#define RMS_SDK_FILE_API_PROTECTORHELPER_H

#include <map>
#include <string>
#include <vector>

namespace rmscore {
namespace fileapi {

enum class ProtectorType {
    PFILE = 0,
    PSTAR = 1,
    OPC = 2,
    MSO = 3,
    PDF = 4,
};

class ProtectorSelector
{
public:
    ProtectorSelector(const std::string& filePath);

    ProtectorType GetProtectorType();

    std::string GetFileExtension();

    std::string GetOutputFileName();

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
