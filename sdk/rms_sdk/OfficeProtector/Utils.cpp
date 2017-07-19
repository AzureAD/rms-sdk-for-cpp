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

// Writes a string to a stream after converting it to a wide string.
void WriteWideStringEntry(GsfOutput *stm, const std::string& entry)
{
    if (stm == nullptr || entry.empty())
    {
        Logger::Error("Invalid arguments provided for writing string entry");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }

    //Doing it this way because wchar_t is 4 bytes on Unix and 2 bytes on Windows.
    std::string wideEntry = ConvertCharStrToWideStr(entry);
    uint32_t wideEntryLen = wideEntry.length();
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&wideEntryLen));
    gsf_output_write(stm, wideEntryLen, reinterpret_cast<const uint8_t*>(wideEntry.data()));
    AlignOutputAtFourBytes(stm, wideEntryLen);
}

// Reads a wide string and converts it to a string.
void ReadWideStringEntry(GsfInput *stm, std::string& entry)
{
    if (stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading string entry");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in reading from stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t wideEntryLen = 0;
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&wideEntryLen));

    if (wideEntryLen % 2 != 0)
    {
        Logger::Error("Corrupt doc file.");
        throw exceptions::RMSMetroOfficeFileException(
                    "Corrupt doc file", exceptions::RMSMetroOfficeFileException::CorruptFile);
    }
    std::unique_ptr<uint8_t[]> wideEntry(new uint8_t[wideEntryLen]);
    gsf_input_read(stm, wideEntryLen, wideEntry.get());
    std::string wideStr((char*)wideEntry.get(), wideEntryLen);
    //Doing it this way because wchar_t is 4 bytes on Unix and 2 bytes on Windows.
    entry = ConvertWideStrToCharStr(wideStr);
    AlignInputAtFourBytes(stm, wideEntryLen);
}

// calculates length of bytes written/read in WriteWideStringEntry()/ReadWideStringEntry()
uint32_t FourByteAlignedWideStringLength(const std::string& entry)
{
    size_t len = sizeof(uint32_t) + (entry.length() << 1);
    // The bitmask is used to round up to the nearest multiple of 4.
    return ((len + 3) & ~3);
}


// Aligns the stream at four bytes. Adds null chars
void AlignOutputAtFourBytes(GsfOutput* stm, uint32_t contentLength)
{
    if (stm == nullptr || contentLength < 1)
    {
        Logger::Error("Invalid arguments provided for byte alignment");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in aligning stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t alignCount = ((contentLength + 3) & ~3) - contentLength;
    std::string alignBytes;

    for(uint32_t i = 0; i < alignCount; i++)
        alignBytes.push_back('\0');

    gsf_output_write(stm, alignCount, reinterpret_cast<const uint8_t*>(alignBytes.data()));
}

// Aligns the stream at four bytes. Seeks to the aligned position while reading.
void AlignInputAtFourBytes(GsfInput *stm, uint32_t contentLength)
{
    if (stm == nullptr || contentLength < 1)
    {
        Logger::Error("Invalid arguments provided for byte alignment");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in aligning stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t alignCount = ((contentLength + 3) & ~3) - contentLength;
    uint64_t pos = gsf_input_tell(stm);
    gsf_input_seek(stm, pos + alignCount, G_SEEK_SET);
}

} // namespace officeprotector
} // namespace rmscore
