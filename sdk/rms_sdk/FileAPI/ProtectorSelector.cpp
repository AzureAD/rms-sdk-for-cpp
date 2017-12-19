/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "ProtectorSelector.h"
#include <vector>
#include <algorithm>
#include <string>
#include "RMSExceptions.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

std::unique_ptr<IProtectorSelector> IProtectorSelector::Create(const std::string& filePath)
{
    std::unique_ptr<IProtectorSelector> protectorSelector(new ProtectorSelector(filePath));
    return protectorSelector;
}

ProtectorSelector::ProtectorSelector(const std::string& filePath)
    : m_fileExtension(".pfile"),
      m_pType(ProtectorType::PFILE)
{
    std::string fileName = filePath.substr(filePath.find_last_of("\\/") + 1);
    Compute(fileName);
}

ProtectorType ProtectorSelector::GetProtectorType()
{
    return m_pType;
}

std::string ProtectorSelector::GetFileExtension()
{
    return m_fileExtension;
}

std::string ProtectorSelector::GetOutputFileName()
{
    return m_newFileName;
}

std::map<std::string, ProtectorType> ProtectorSelector::Init()
{
    static std::map<std::string, ProtectorType> protectorExtensionsMap = {
        {".docm", ProtectorType::OPC},{".docx", ProtectorType::OPC},{".dotm", ProtectorType::OPC},
        {".dotx", ProtectorType::OPC},{".xlam", ProtectorType::OPC},{".xlsb", ProtectorType::OPC},
        {".xlsm", ProtectorType::OPC},{".xlsx", ProtectorType::OPC},{".xltx", ProtectorType::OPC},
        {".xps", ProtectorType::OPC},{".potm", ProtectorType::OPC},{".potx", ProtectorType::OPC},
        {".ppsx", ProtectorType::OPC},{".ppsm", ProtectorType::OPC},{".pptm", ProtectorType::OPC},
        {".pptx", ProtectorType::OPC},{".thmx", ProtectorType::OPC},{".vsdx", ProtectorType::OPC},
        {".vsdm", ProtectorType::OPC},{".vssx", ProtectorType::OPC},{".vssm", ProtectorType::OPC},
        {".vstx", ProtectorType::OPC},{".vstm", ProtectorType::OPC},{".jt", ProtectorType::PSTAR},
        {".txt", ProtectorType::PSTAR},{".xm", ProtectorType::PSTAR},{".jpg", ProtectorType::PSTAR},
        {".jpeg", ProtectorType::PSTAR},{".png", ProtectorType::PSTAR},{".tif", ProtectorType::PSTAR},
        {".tiff", ProtectorType::PSTAR},{".bmp", ProtectorType::PSTAR},{".gif", ProtectorType::PSTAR},
        {".jpe", ProtectorType::PSTAR},{".jfif", ProtectorType::PSTAR},{".jif", ProtectorType::PSTAR},
        {".pdf", ProtectorType::PDF}};

    return protectorExtensionsMap;
}

const std::map<std::string, ProtectorType>& ProtectorSelector::GetProtectorExtensionsMap()
{
    static std::map<std::string, ProtectorType> protectorExtensionsMap = Init();
    return protectorExtensionsMap;
}

void ProtectorSelector::Compute(const std::string& fileName)
{
    if (fileName.empty())
    {
        throw exceptions::RMSInvalidArgumentException("The filename is empty");
    }

    auto pos = fileName.find_last_of('.');
    if (pos == std::string::npos)
    {
        Logger::Error("Invalid filename provided.", fileName.c_str());
        throw exceptions::RMSInvalidArgumentException(
                    "Full filename with extension needed. Filename provided: " + fileName);
    }

    std::string ext = fileName.substr(pos);    
    if (ext == ".pfile")
    {
        m_newFileName = fileName.substr(0, pos);
        return;
    }

    ProtectorType pType = ProtectorType::PFILE;
    auto protectorExtensionsMap = GetProtectorExtensionsMap();

    if(protectorExtensionsMap.find(ext) != protectorExtensionsMap.end())  //Key present
    {
        pType = protectorExtensionsMap[ext];
    }

    if (pType != ProtectorType::PFILE && pType != ProtectorType::PSTAR)   //Native protector
    {
        m_pType = pType;
        m_newFileName = fileName;
        m_fileExtension = ext;
        return;
    }

    auto ppos = ext.find_first_of('p');
    if (pType == ProtectorType::PSTAR)  //PStar extension protection
    {
        m_pType = ProtectorType::PSTAR;
        m_fileExtension = ext;
        m_newFileName = fileName.substr(0, pos+1) + "p" + ext.substr(1);
    }
    else if (ppos != std::string::npos) // PStar extension unprotection
    {
        m_pType = ProtectorType::PSTAR;
        m_fileExtension = ext.substr(ppos + 1);
        m_newFileName = fileName.substr(0, pos+1) + m_fileExtension;
    }
    else                                // Pfile extension
    {
        m_fileExtension = ext;
        m_newFileName = fileName + ".pfile";
    }
}

} // namespace fileapi
} // namespace rmscore
