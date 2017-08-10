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
using namespace std;
uint64_t    timeToWinFileTime(const QDateTime& dateTime);
std::string timeToString(const QDateTime& dateTime);
vector<uint8_t>   ConvertBase64ToBytes(const vector<uint8_t>& base64str);
vector<uint8_t>   ConvertBytesToBase64(const vector<uint8_t>& bytes);
vector<uint8_t>   ConvertBytesToBase64(const void  *bytes,
                                 const size_t size);
std::string ReplaceString(std::string subject,
                          const std::string& search,
                          const std::string& replace, int occurrences = 0);

inline bool isJson(std::string document) { return document[0] == '{'; }

template<typename OP, typename... Args>
inline void operate_many(OP func, Args&&... args)
{
    for (auto o : {std::forward<Args>(args)...})
        func(o);
}

vector<uint8_t> HashString(const vector<uint8_t> &bytes, size_t *size, bool isSHA256 = true);
std::string Hash(const std::string& str, size_t *size);
std::string GenerateAGuid();
std::string Unescape(std::string source, bool skipReformat = false);
} // namespace common
} // namespace rmscore
#endif // _RMS_LIB_TOOLS_H_
