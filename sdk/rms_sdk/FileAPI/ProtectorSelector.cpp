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

ProtectorSelector::ProtectorSelector(const std::string& fileName)
{
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

void ProtectorSelector::Init()
{
//    m_nativeProtectorExtensions[ProtectorType::MSO] = { ".doc", ".dot", ".xla", ".xls", ".xlt",
//                                                        ".pps", ".ppt", ".pot" };
    m_nativeProtectorExtensions[ProtectorType::OPC] = { ".docm", ".docx", ".dotm", ".dotx", ".xlam",
                                                        ".xlsb", ".xlsm", ".xlsx", ".xltm", ".xltx",
                                                        ".xps", ".potm", ".potx", ".ppsx", ".ppsm",
                                                        ".pptm", ".pptx", ".thmx", ".vsdx", ".vsdm",
                                                        ".vssx", ".vssm", ".vstx", ".vstm" };
//    m_nativeProtectorExtensions[ProtectorType::PDF] = { ".pdf" };
    m_pStarExtensions = {".jt", ".txt",".xm",".jpg",".jpeg",".png",".tif",".tiff",".bmp",
                         ".gif",".jpe",".jfif",".jif", ".pdf"};
}

void ProtectorSelector::Compute(std::string fileName)
{
    if (fileName.empty())
    {
        throw exceptions::RMSInvalidArgumentException("The filename is empty");
    }

    auto pos = fileName.find_last_of('.');
    if (pos == std::string::npos)
    {
        Logger::Error("Invalid filename provided.", fileName);
        throw exceptions::RMSInvalidArgumentException("Full filename with extension needed");
    }

    std::string ext = fileName.substr(pos);
    ProtectorType pType = ProtectorType::PFILE;
    if (m_nativeProtectorExtensions.size() == 0)
    {
        Init();
    }

    for (auto it = std::begin(m_nativeProtectorExtensions);
         it != std::end(m_nativeProtectorExtensions); ++it)
    {
        auto vectorIterator = std::find (it->second.begin(), it->second.end(), ext);
        if (vectorIterator != std::end(it->second))
        {
            pType = it->first;
            break;
        }
    }

    if (pType != ProtectorType::PFILE)   //Native protector
    {
        m_pType = pType;
        m_newFileName = fileName;
        m_fileExtension = ext;
        return;
    }

    m_pType = ProtectorType::PFILE;
    if (ext == ".pfile")
    {
        m_newFileName = fileName.substr(0, pos);
        m_fileExtension = ".pfile";
        return;
    }

    auto it = std::find (m_pStarExtensions.begin(), m_pStarExtensions.end(), ext);
    auto ppos = ext.find_first_of('p');
    if (it != m_pStarExtensions.end())  //PStar extension protection
    {
        m_fileExtension = ext;
        m_newFileName = fileName.substr(0, pos+1) + "p" + ext.substr(1);
    }
    else if (ppos != std::string::npos) // PStar extension unprotection
    {
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
