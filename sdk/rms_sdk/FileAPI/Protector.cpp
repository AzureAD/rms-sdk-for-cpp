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
#include "PDFProtector.h"
#include "ProtectorSelector.h"
#include "RMSExceptions.h"
#include "UserPolicy.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

std::unique_ptr<Protector> Protector::Create(const std::string& filePath,
                                             std::shared_ptr<std::fstream> input_stream,
                                             std::string& outputFileName)
{
    if (!input_stream->is_open())
    {
        throw exceptions::RMSInvalidArgumentException("The input stream is invalid");
    }

    ProtectorSelector protectorSelector(filePath);
    ProtectorType pType = protectorSelector.GetProtectorType();
    outputFileName = protectorSelector.GetOutputFileName();
    switch (pType)
    {
        case ProtectorType::OPC:
        {
            std::unique_ptr<Protector> protector(new MetroOfficeProtector(input_stream));
            return protector;
        }
        break;

        case ProtectorType::PFILE:
        case ProtectorType::PSTAR:
        {
            std::unique_ptr<Protector> protector(new PFileProtector(
                                                      protectorSelector.GetFileExtension(),
                                                      input_stream));
            return protector;
        }
        break;

        case ProtectorType::PDF:
        {
            std::unique_ptr<Protector> protector(new PDFProtector(
                                                  filePath,
                                                  protectorSelector.GetFileExtension(),
                                                  input_stream));
            return protector;
        }
        break;

        default:
        {
            throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                "");
        }
        break;
    }

    return nullptr;
}

std::unique_ptr<ProtectorWithWrapper> ProtectorWithWrapper::Create(const std::string& filePath,
                                             std::shared_ptr<std::fstream> input_stream,
                                             std::string& outputFileName)
{
    if (!input_stream->is_open())
    {
        throw exceptions::RMSInvalidArgumentException("The input stream is invalid");
    }

    ProtectorSelector protectorSelector(filePath);
    ProtectorType pType = protectorSelector.GetProtectorType();
    outputFileName = protectorSelector.GetOutputFileName();
    switch (pType)
    {
        case ProtectorType::PDF:
        {
            std::unique_ptr<ProtectorWithWrapper> protector(new PDFProtector(
                                                  filePath,
                                                  protectorSelector.GetFileExtension(),
                                                  input_stream));
            return protector;
        }
        break;

        default:
        {
            throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                                "This file format is not supported.");
        }
        break;
    }
    return nullptr;
}

} // namespace fileapi
} // namespace rmscore
