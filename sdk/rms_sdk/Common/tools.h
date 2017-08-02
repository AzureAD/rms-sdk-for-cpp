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
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>

namespace rmscore {
namespace common {

// Turns out that there is no platform-independent, re-entrant way of converting
// time_t to tm.
// This platform independent re-entrant function converts the given time_t to
// tm for GMT time zone.
tm ConvertStdTimeToGmtTm(time_t time);

// Converts the given time to a string with the given format.
std::string ConvertTmToString(const tm& t, const std::string& format);

// Returns current time in GMT as string based on the given format string.
std::string GetCurrentGmtAsString(const std::string& format);

std::time_t GetTimeFromString(const std::string& dateTime, const std::string& format);

uint64_t    timeToWinFileTime(const time_t &dateTime);
ByteArray   ConvertBase64ToBytes(const ByteArray& base64str);
ByteArray   ConvertBytesToBase64(const ByteArray& bytes);
ByteArray   ConvertBytesToBase64(const void  *bytes,
                                 const size_t size);
std::string GenerateAGuid();
} // namespace common
} // namespace rmscore
#endif // _RMS_LIB_TOOLS_H_
