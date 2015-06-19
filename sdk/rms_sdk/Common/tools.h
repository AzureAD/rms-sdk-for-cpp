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

namespace rmscore {
namespace common {
uint64_t    timeToWinFileTime(const QDateTime& dateTime);
std::string timeToString(const QDateTime& dateTime);
ByteArray   ConvertBase64ToBytes(const ByteArray& base64str);
ByteArray   ConvertBytesToBase64(const ByteArray& bytes);
ByteArray   ConvertBytesToBase64(const void  *bytes,
                                 const size_t size);
std::string GenerateAGuid();
} // namespace common
} // namespace rmscore
#endif // _RMS_LIB_TOOLS_H_
