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

ProtectorSelector::ProtectorSelector(const std::string &fileName)
{
    if(fileName.empty())
    {
        throw exceptions::RMSInvalidArgumentException("The filename is empty");
    }
    m_fileName = fileName;
}

ProtectorType ProtectorSelector::GetProtectorType()
{
    if(m_newFileName.empty())
    {
        Compute();
    }
    return m_pType;
}

std::string ProtectorSelector::GetFileExtension()
{
    if(m_fileExtension.empty())
    {
        Compute();
    }
    return m_fileExtension;
}

std::string ProtectorSelector::GetOutputFileName()
{
    if(m_newFileName.empty())
    {
        Compute();
    }
    return m_newFileName;
}

void ProtectorSelector::Init()
{
    m_nativeProtectorExtensions[ProtectorType::Mso] = { ".doc", ".dot", ".xla", ".xls", ".xlt",
                                                        ".pps", ".ppt", ".pot" };
    m_nativeProtectorExtensions[ProtectorType::Opc] = { ".docm", ".docx", ".dotm", ".dotx", ".xlam",
                                                        ".xlsb", ".xlsm", ".xlsx", ".xltm", ".xltx",
                                                        ".xps", ".potm", ".potx", ".ppsx", ".ppsm",
                                                        ".pptm", ".pptx", ".thmx", ".vsdx", ".vsdm",
                                                        ".vssx", ".vssm", ".vstx", ".vstm" };
    m_nativeProtectorExtensions[ProtectorType::Pdf] = { ".pdf" };
    m_pStarExtensions = {".jt", ".txt",".xm",".jpg",".jpeg",".png",".tif",".tiff",".bmp",
                         ".gif",".jpe",".jfif",".jif"};
}

void ProtectorSelector::Compute()
{
    std::string ext = "";
    auto pos = m_fileName.find_last_of('.');
    if (pos == std::string::npos)
    {
        Logger::Error("Invalid filename provided.", m_fileName);
        throw exceptions::RMSInvalidArgumentException("Full filename with extension needed");
    }

    ext = m_fileName.substr(pos);
    ProtectorType pType = ProtectorType::Pfile;
    if(m_nativeProtectorExtensions.size() == 0)
    {
        Init();
    }

    for( auto it = std::begin(m_nativeProtectorExtensions);
         it != std::end(m_nativeProtectorExtensions); ++it)
    {
        auto vectorIterator = std::find (it->second.begin(), it->second.end(), ext);
        if(vectorIterator != std::end(it->second))
        {
            pType = it->first;
            break;
        }
    }

    if(pType != ProtectorType::Pfile)   //Native protector
    {
        if(pType == ProtectorType::Mso)   //Temporary until we add support for old office format
        {
            m_pType = ProtectorType::Pfile;
            m_newFileName = m_fileName + ".pfile";
        }
        else if(pType == ProtectorType::Pdf)  //Temporary until we add support for pdf format
        {
            m_pType = ProtectorType::Pfile;
            m_newFileName = m_fileName.substr(0, pos+1) + "p" + ext.substr(1);
        }
        else
        {
            m_pType = pType;
            m_newFileName = m_fileName;
        }

        m_fileExtension = ext;
        return;
    }

    m_pType = ProtectorType::Pfile;
    if(ext.compare(".pfile") == 0)
    {
        m_newFileName = m_fileName.substr(0, pos);
        m_fileExtension = ".pfile";
        return;
    }

    auto ppos = ext.find_first_of('p');
    if (ppos != std::string::npos)
    {
      m_fileExtension = ext.substr(ppos + 1);
      m_newFileName = m_fileName.substr(0, pos+1) + m_fileExtension;
    }
    else
    {
        auto it = std::find (m_pStarExtensions.begin(), m_pStarExtensions.end(), ext);
        if (it != m_pStarExtensions.end())  //PStar extension
        {
            m_fileExtension = ext;
            m_newFileName = m_fileName.substr(0, pos+1) + "p" + ext.substr(1);
        }
        else
        {
            m_fileExtension = ".pfile";
            m_newFileName = m_fileName + m_fileExtension;
        }
    }
}

} // namespace fileapi
} // namespace rmscore
