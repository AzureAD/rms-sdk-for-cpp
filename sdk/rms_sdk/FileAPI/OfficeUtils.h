/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef RMS_SDK_FILEAPI_OFFICEUTILS_H
#define RMS_SDK_FILEAPI_OFFICEUTILS_H

#include <cstdio>
#include <string>
#include "FileAPIStructures.h"
#include "UserPolicy.h"
#include <gsf/gsf.h>

namespace rmscore {
namespace fileapi {

// 1 GB is the max size of a file we can encrypt
static const uint64_t MAX_FILE_SIZE_FOR_ENCRYPT = 1ul << 30;
// 3 GB is the max size of a file we can decrypt
static const uint64_t MAX_FILE_SIZE_FOR_DECRYPT = 3ul << 30;
static const uint64_t BUF_SIZE_BYTES = 4096;
typedef std::string tempFileName;

void WriteStreamHeader(GsfOutput* stm, const uint64_t& contentLength);

void ReadStreamHeader(GsfInput* stm, uint64_t& contentLength);

modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptions(
        const bool& allowAuditedExtraction,
        CryptoOptions cryptoOptions);

void CopyFromFileToOstream(std::string fileName, std::ostream* stream);

void CopyFromIstreamToFile(std::istream *stream, const std::string& tempFileName,
                           uint64_t inputFileSize);

std::string CreateTemporaryFileName(const std::string& fileName);

uint64_t ValidateAndGetFileSize(std::istream* stream, uint64_t maxFileSize);

uint64_t ValidateAndGetFileSize(FILE* file, uint64_t maxFileSize);

struct FILE_deleter
{
    void operator () (FILE* obj) const
    {
        std::fclose(obj);
    }
};

struct tempFile_deleter
{
    void operator () (tempFileName* obj) const
    {
        remove(obj->c_str());
    }
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILEAPI_OFFICEUTILS_H
