/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/


#ifndef RMS_SDK_FILE_API_PROTECTORFACTORY_H
#define RMS_SDK_FILE_API_PROTECTORFACTORY_H

#include <map>
#include <set>
#include <string>
#include "IProtector.h"

namespace rmscore {
namespace fileapi {

enum ProtectorType {

    Pfile = 0,
    Opc = 1,
    Mso = 2,
    Pdf = 3,
    PStar = 4,
};

class DLL_PUBLIC_RMS ProtectorFactory
{
public:
    static std::shared_ptr<IProtector> CreateProtector(const std::string& filename,
                                                       bool isProtect);

private:
    static void Init();

    static ProtectorType ComputeProtectorType(const std::string& filename,
                                              bool isProtect);

    static std::map<ProtectorType, std::set<std::string>> m_protectorExtensions;
};

} // namespace officeprotector
} // namespace fileapi

#endif // RMS_SDK_FILE_API_PROTECTORFACTORY_H
