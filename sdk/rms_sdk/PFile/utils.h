/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef RMS_SDK_PFILE_UTILS_H
#define RMS_SDK_PFILE_UTILS_H

#include <cstdio>
#include <iostream>
#include <string>
#include "file_api_structures.h"
#include "UserPolicy.h"

namespace rmscore {
namespace utils {

using namespace rmscore::fileapi;

// 1 GB is the max size of a file we can encrypt
static const uint64_t MAX_FILE_SIZE_FOR_ENCRYPT = 1ul << 30;
// 3 GB is the max size of a file we can decrypt
static const uint64_t MAX_FILE_SIZE_FOR_DECRYPT = 3ul << 30;
static const uint64_t BUF_SIZE_BYTES = 4096;

typedef std::string TempFileName;

/*!
 * \brief Uses 'cryptoOptions' and 'allowAuditedExtraction' params to create enum value
 * \param allowAuditedExtraction
 * \param cryptoOptions
 * \return An enum which has the required user policy creation options
 */
modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptionsForPfile(
    const bool& allowAuditedExtraction,
    CryptoOptions cryptoOptions);

/*!
 * \brief Opens file with 'filename' and copies the contents as binary to 'stream'
 * \param fileName
 * \param stream
 */
void CopyFromFileToOstream(std::string fileName, std::ostream* stream);

/*!
 * \brief Creates a file with name 'fileName' and copies contents from 'stream' as binary.
 * \param stream
 * \param tempFileName
 * \param inputFileSize The size of the stream pointed to by 'stream'
 */
void CopyFromIstreamToFile(
    std::istream *stream,
    const std::string& fileName,
    uint64_t inputFileSize);

/*!
 * \brief Creates a name for a temporary file by concatenating 'filename' with a random number
 * \param fileName
 * \return A random string which is used as the name of the temporary file
 */
std::string CreateTemporaryFileName(const std::string& fileName);

/*!
 * \brief Calculates the size of the stream pointed to by 'stream' and throws if it is more than
 *        'maxFileSize'
 * \param stream
 * \param maxFileSize
 * \return The size of the stream
 */
uint64_t ValidateAndGetFileSize(std::istream* stream, uint64_t maxFileSize);

/*!
 * \brief Calculates the size of the stream pointed to by 'file' and throws if it is more than
 *        'maxFileSize'
 * \param stream
 * \param maxFileSize
 * \return The size of the stream
 */
uint64_t ValidateAndGetFileSize(FILE* file, uint64_t maxFileSize);

void CheckStream(std::ios* stream, std::string message);

struct FILE_closer {
  void operator () (FILE* obj) const {
    std::fclose(obj);
  }
};

struct TempFile_deleter {
  void operator () (TempFileName* obj) const {
    remove(obj->c_str());
  }
};

} // namespace utils
} // namespace rmscore

#endif // RMS_SDK_PFILE_UTILS_H
