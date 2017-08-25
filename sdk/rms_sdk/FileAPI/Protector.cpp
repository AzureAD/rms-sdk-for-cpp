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
#include "MsoOfficeProtector.h"
#include "PFileProtector.h"
#include "ProtectorSelector.h"
#include "RMSExceptions.h"
#include "UserPolicy.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;
using namespace rmscore::pfile;
using namespace rmscore::officeprotector;

namespace rmscore {
namespace fileapi {

std::unique_ptr<Protector> Protector::Create(const std::string& fileName,
                                             std::shared_ptr<std::istream> inputStream,
                                             std::string& outputFileName)
{
    if (!inputStream->good())
    {
        throw exceptions::RMSInvalidArgumentException("The input stream is invalid");
    }

    ProtectorSelector protectorSelector(fileName);
    ProtectorType pType = protectorSelector.GetProtectorType();
    outputFileName = protectorSelector.GetOutputFileName();
    switch (pType)
    {
        case ProtectorType::OPC:
        {
            std::unique_ptr<Protector> protector(new MetroOfficeProtector(fileName,
                                                                          inputStream));
            return protector;
        }        

        case ProtectorType::MSO:
        {
            std::unique_ptr<Protector> protector(new MsoOfficeProtector(fileName, inputStream));

            return protector;
        }

        case ProtectorType::PFILE:
        case ProtectorType::PSTAR:
        {
            std::unique_ptr<Protector> protector(new PFileProtector(
                                                     protectorSelector.GetCurrentFileExtension(),
                                                     inputStream));
            return protector;
        }        

        default:
        {
            throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                "");
        }
        break;
    }
}

} // namespace fileapi
} // namespace rmscore
