/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "Utils.h"
#include "../Platform/Logger/Logger.h"
#include "../ModernAPI/RMSExceptions.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace officeprotector {

// Adds a '\0' after every char to make for MSIPC wstring compatibility.
// "RMS" becomes "R\0M\0S\0"
std::string ConvertCharStrToWideStr(const std::string& input)
{
    size_t inputLen = input.length();
    std::string wideStr(2 * inputLen, '\0');
    for(size_t i = 0; i < inputLen; i++)
    {
        wideStr[2 * i] = input[i];
    }
    return wideStr;
}

// Removes a '\0' present after every char to make for MSIPC wstring compatibility.
// "R\0M\0S\0" becomes "RMS"
std::string ConvertWideStrToCharStr(const std::string& input)
{
    size_t inputLen = input.length();
    std::string charStr(inputLen / 2, '\0');
    for(size_t i = 0; i < inputLen; i += 2)
    {
        charStr[i / 2] = input[i];
    }
    return charStr;
}

// Aligns the stream at four bytes. Adds null chars while writing
// and seeks to the aligned position while reading.
uint32_t AlignAtFourBytes(std::shared_ptr<pole::Stream> stm, uint32_t contentLength, bool write)
{
    if(stm == nullptr || contentLength < 1)
    {
        Logger::Error("Invalid arguments provided for byte alignment");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in aligning stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t alignCount = ((contentLength + 3) & ~3) - contentLength;
    std::string alignBytes;

    if(write)
    {
        for(uint32_t i = 0; i < alignCount; i++)
            alignBytes.push_back('\0');

        stm->write(reinterpret_cast<unsigned char*>(const_cast<char*>(alignBytes.data())),
                   alignCount);
    }
    else
    {
        uint64_t pos = stm->tell();
        stm->seek(pos + alignCount);
    }
    return alignCount;
}

// Writes a string to a stream after converting it to a wide string.
uint32_t WriteWideStringEntry(std::shared_ptr<pole::Stream> stm, const std::string& entry)
{
    if(stm == nullptr || entry.empty())
    {
        Logger::Error("Invalid arguments provided for writing string entry");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t bytesWritten = 0;
    //Doing it this way because wchar_t is 4 bytes on Unix and 2 bytes on Windows.
    std::string wideEntry = ConvertCharStrToWideStr(entry);
    uint32_t wideEntryLen = wideEntry.length();
    bytesWritten += stm->write(reinterpret_cast<unsigned char*>(&wideEntryLen),
                               sizeof(uint32_t));
    bytesWritten += stm->write(reinterpret_cast<unsigned char*>(const_cast<char*>(wideEntry.data())),
                               wideEntryLen);
    bytesWritten += AlignAtFourBytes(stm, wideEntryLen, true);

    return bytesWritten;
}

// Reads a wide string and converts it to a string.
uint32_t ReadWideStringEntry(std::shared_ptr<pole::Stream> stm, std::string& entry)
{
    if(stm == nullptr || entry.empty())
    {
        Logger::Error("Invalid arguments provided for reading string entry");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in reading from stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t bytesRead = 0;
    uint32_t wideEntryLen = 0;
    bytesRead += stm->read(reinterpret_cast<unsigned char*>(&wideEntryLen), sizeof(uint32_t));

    if(wideEntryLen % 2 != 0)
    {
        Logger::Error("Corrupt doc file.");
        throw exceptions::RMSMetroOfficeFileException(
                    "Corrupt doc file", exceptions::RMSMetroOfficeFileException::CorruptFile);
    }
    auto wideEntry = std::make_unique<unsigned char[]>(wideEntryLen);
    bytesRead += stm->read(wideEntry.get(), wideEntryLen);
    std::string wideStr((char*)wideEntry.get(), wideEntryLen);
    //Doing it this way because wchar_t is 4 bytes on Unix and 2 bytes on Windows.
    entry = ConvertWideStrToCharStr(wideStr);
    bytesRead += AlignAtFourBytes(stm, wideEntryLen, false);

    return bytesRead;
}

// calculates length of bytes written/read in WriteWideStringEntry()/ReadWideStringEntry()
uint32_t FourByteAlignedWideStringLength(const std::string& entry)
{
    size_t len = sizeof(uint32_t) + (entry.length() << 1);
    // The bitmask is used to round up to the nearest multiple of 4.
    return ((len + 3) & ~3);
}

} // namespace officeprotector
} // namespace rmscore
