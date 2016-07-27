/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_TOOLS_H_
#define _RMS_LIB_TOOLS_H_

#include "CommonTypes.h"
#include "FrameworkSpecificTypes.h"
#include "openssl/sha.h"
#include <stdint.h>
#include <string>
#include <sstream>
#include <iomanip>

namespace rmscore {
namespace common {

uint64_t    timeToWinFileTime(const QDateTime& dateTime);
std::string timeToString(const QDateTime& dateTime);
ByteArray   ConvertBase64ToBytes(const ByteArray& base64str);
ByteArray   ConvertBytesToBase64(const ByteArray& bytes);
ByteArray   ConvertBytesToBase64(const void  *bytes,
                                 const size_t size);
std::string ReplaceString(std::string subject,
                          const std::string& search,
                          const std::string& replace);

template <typename T> inline std::vector<T> ConvertArrayToVector(T* buf, const size_t len) { return std::vector<T>(buf, buf + len); }
std::unique_ptr<uint8_t[]> HashString(const std::string& str, size_t *size);
std::string GenerateAGuid();
} // namespace common
} // namespace rmscore
#endif // _RMS_LIB_TOOLS_H_
