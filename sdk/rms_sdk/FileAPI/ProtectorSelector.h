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
    OPC = 1,
    MSO = 2,
    PDF = 3,
};

class ProtectorSelector
{
public:
    ProtectorSelector(const std::string& fileName);

    ProtectorType GetProtectorType();

    std::string GetFileExtension();

    std::string GetOutputFileName();

private:
    void Init();

    void Compute(std::string fileName);

    std::map<ProtectorType, std::vector<std::string>> m_nativeProtectorExtensions;
    std::vector<std::string> m_pStarExtensions;
    std::string m_fileExtension;
    std::string m_newFileName;
    ProtectorType m_pType;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PROTECTORHELPER_H
