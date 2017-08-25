/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "OfficeUtils.h"
#include <codecvt>
#include <locale>
#include "../Platform/Logger/Logger.h"
#include "../ModernAPI/RMSExceptions.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace officeutils {

// Known issue with Visual C++. Please refer
// https://connect.microsoft.com/VisualStudio/feedback/details/1348277/link-error-when-using-std-codecvt-utf8-utf16-char16-t

#if _MSC_VER >= 1900

std::string utf16_to_utf8(const std::u16string& utf16_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
    auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
    return convert.to_bytes(p, p + utf16_string.size());
}

std::u16string utf8_to_utf16(const std::string& utf8_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;

    auto str = convert.from_bytes(utf8_string);
    std::u16string result{ str.begin(), str.end() };
    return result;
}

#else

std::string utf16_to_utf8(const std::u16string& utf16_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(utf16_string);
}

std::u16string utf8_to_utf16(const std::string& utf8_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.from_bytes(utf8_string);
}

#endif

void WriteWideStringEntry(GsfOutput *stm, const std::string& entry)
{
    if (stm == nullptr || entry.empty())
    {
        Logger::Error("Invalid arguments provided for writing string entry");
        throw exceptions::RMSOfficeFileException(
                    "Error in writing to stream", exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    auto entry_utf16 = utf8_to_utf16(entry);
    uint32_t entry_utf16_len = entry_utf16.length() * 2;
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&entry_utf16_len));
    gsf_output_write(stm, entry_utf16_len, reinterpret_cast<const uint8_t*>(entry_utf16.data()));
    AlignOutputAtFourBytes(stm, entry_utf16_len);
}

void ReadWideStringEntry(GsfInput *stm, std::string& entry)
{
    if (stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading string entry");
        throw exceptions::RMSOfficeFileException(
                    "Error in reading from stream", exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    uint32_t entry_utf16_len = 0;
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&entry_utf16_len));

    if (entry_utf16_len % 2 != 0)
    {
        Logger::Error("Corrupt doc file.");
        throw exceptions::RMSOfficeFileException(
                    "Corrupt doc file", exceptions::RMSOfficeFileException::Reason::CorruptFile);
    }
    std::vector<uint8_t> ent(entry_utf16_len);
    gsf_input_read(stm, entry_utf16_len, &ent[0]);
    std::u16string entry_utf16(reinterpret_cast<const char16_t*>(ent.data()), (ent.size()+1)/2);
    entry = utf16_to_utf8(entry_utf16);
    AlignInputAtFourBytes(stm, entry_utf16_len);
}

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
        throw exceptions::RMSOfficeFileException(
                    "Error in aligning stream", exceptions::RMSOfficeFileException::Reason::Unknown);
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
        throw exceptions::RMSOfficeFileException(
                    "Error in aligning stream", exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    uint32_t alignCount = ((contentLength + 3) & ~3) - contentLength;
    uint64_t pos = gsf_input_tell(stm);
    gsf_input_seek(stm, pos + alignCount, G_SEEK_SET);
}

void WriteStreamHeader(GsfOutput* stm, const uint64_t& contentLength)
{
    if ( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing stream header");
        throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::StreamError,
                                            "Error in writing to stream");
    }
    gsf_output_seek(stm, 0, G_SEEK_SET);
    gsf_output_write(stm, sizeof(uint64_t), reinterpret_cast<const uint8_t*>(&contentLength));
}

void ReadStreamHeader(GsfInput* stm, uint64_t& contentLength)
{
    if ( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading stream header");
        throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::StreamError,
                                            "Error in reading from stream");
    }
    gsf_input_seek(stm, 0, G_SEEK_SET);
    gsf_input_read(stm, sizeof(uint64_t), reinterpret_cast<uint8_t*>(&contentLength));
}

} // namespace officeutils
} // namespace rmscore
