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
#include "ProtectorSelector.h"
#include "RMSExceptions.h"
#include "UserPolicy.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

std::unique_ptr<Protector> Protector::Create(const std::string& fileName,
                                             std::shared_ptr<std::fstream> inputStream,
                                             std::string& outputFileName)
{
    if (!inputStream->is_open())
    {
        throw exceptions::RMSInvalidArgumentException("The input stream is invalid");
    }

    std::unique_ptr<Protector> protector;
    ProtectorSelector protectorSelector(fileName);
    ProtectorType pType = protectorSelector.GetProtectorType();
    outputFileName = protectorSelector.GetOutputFileName();
    switch (pType)
    {
        case ProtectorType::OPC:
        {
            protector = std::make_unique<MetroOfficeProtector>(inputStream);
        }
        break;

        case ProtectorType::PFILE:
        {
            protector = std::make_unique<PFileProtector>(protectorSelector.GetFileExtension(),
                                                         inputStream);
        }
        break;

        default:
        {
            throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                "");
        }
        break;
    }
    return protector;
}

} // namespace fileapi
} // namespace rmscore
