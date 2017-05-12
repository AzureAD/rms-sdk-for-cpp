/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <Utils.h>
#include "../Platform/Logger/Logger.h"
#include "../ModernAPI/RMSExceptions.h"
using namespace rmscore::pole;
using namespace rmscore::platform::logger;
namespace rmscore {
namespace officeprotector {

//doing it this way because wchar_t is 4 bytes on Unix and 2 bytes on Windows.
std::string ConvertCharStrToWideStr(std::string input)
{
    size_t inputLen = input.length();
    std::string wideStr;
    for(size_t i=0; i < inputLen; i++)
    {
        wideStr.push_back(input[i]);
        wideStr.push_back('\0');
    }
    return wideStr;
}

std::string ConvertWideStrToCharStr(std::string input)
{
    size_t inputLen = input.length();
    std::string CharStr;
    for(size_t i=0; i < inputLen; i+=2)
    {
        CharStr.push_back(input[i]);
    }
    return CharStr;
}


uint32_t FourByteAlign(Stream *stm, uint32_t contentLength, bool write)
{
    if(stm == nullptr || contentLength < 1)
    {
        Logger::Error("Invalid arguments provided for byte alignment");
        throw exceptions::RMSMetroOfficeFileException("Error in aligning stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t alignCount = contentLength % 4;
    std::string alignBytes;

    if(write)
    {
        for(uint32_t i=0; i < alignCount; i++)
            alignBytes.push_back('\0');

        stm->write(reinterpret_cast<unsigned char*>(const_cast<char*>(alignBytes.data())), alignCount);
    }
    else
    {
        uint64_t pos = stm->tell();
        stm->seek(pos + alignCount);
    }
    return alignCount;
}

uint32_t WriteWideStringEntry(Stream *stm, std::string entry)
{
    if(stm == nullptr || entry.empty())
    {
        Logger::Error("Invalid arguments provided for writing string entry");
        throw exceptions::RMSMetroOfficeFileException("Error in writing to stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t bytesWritten = 0;
    std::string wideEntry = ConvertCharStrToWideStr(entry);
    uint32_t wideEntryLen = wideEntry.length();
    bytesWritten += stm->write(reinterpret_cast<unsigned char*>(&wideEntryLen), sizeof(uint32_t));
    bytesWritten += stm->write(reinterpret_cast<unsigned char*>(const_cast<char*>(wideEntry.data())), wideEntryLen);
    bytesWritten += FourByteAlign(stm, wideEntryLen, true);

    return bytesWritten;
}

uint32_t ReadWideStringEntry(Stream *stm, std::string &entry)
{
    if(stm == nullptr || entry.empty())
    {
        Logger::Error("Invalid arguments provided for reading string entry");
        throw exceptions::RMSMetroOfficeFileException("Error in reading from stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }
    uint32_t bytesRead = 0;
    uint32_t wideEntryLen = 0;
    bytesRead += stm->read(reinterpret_cast<unsigned char*>(&wideEntryLen), sizeof(uint32_t));

    if(wideEntryLen % 2 != 0)
    {
        Logger::Error("Corrupt doc file.");
        throw exceptions::RMSMetroOfficeFileException("Corrupt doc file",
                                                      exceptions::RMSMetroOfficeFileException::CorruptFile);
    }
    unsigned char *wideEntry = new unsigned char[wideEntryLen];
    bytesRead += stm->read(wideEntry, wideEntryLen);
    std::string wideStr(reinterpret_cast<const char*>(wideEntry), wideEntryLen);
    entry = ConvertWideStrToCharStr(wideStr);
    bytesRead += FourByteAlign(stm, wideEntryLen, false);
    delete wideEntry;

    return bytesRead;
}

uint32_t WideStringEntryLength(std::string entry)
{
    size_t len = sizeof(uint32_t) + (entry.length()*2);
    return ((len + 3) & ~3);
}

} // namespace officeprotector
} //namespace rmscore
