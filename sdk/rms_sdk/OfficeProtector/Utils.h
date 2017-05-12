/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_IRMUTILS_H
#define _RMS_LIB_IRMUTILS_H

#include <string>
#include <pole.h>
using namespace rmscore::pole;
namespace rmscore {
namespace officeprotector {

uint32_t WriteWideStringEntry(Stream *stm, std::string entry);
uint32_t ReadWideStringEntry(Stream *stm, std::string &entry);
uint32_t WideStringEntryLength(std::string entry);
uint32_t FourByteAlign(Stream *stm, uint32_t contentLength, bool write);
std::string ConvertCharStrToWideStr(std::string input);
std::string ConvertWideStrToCharStr(std::string input);


} // namespace officeprotector
} //namespace rmscore
#endif // _RMS_LIB_IRMUTILS_H
