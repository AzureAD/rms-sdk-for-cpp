/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef RMS_SDK_OFFICE_PROTECTOR_IRMUTILS_H
#define RMS_SDK_OFFICE_PROTECTOR_IRMUTILS_H

#include <string>
#include <gsf/gsf.h>

namespace rmscore {
namespace officeprotector {

void WriteWideStringEntry(GsfOutput* stm, const std::string& entry);
void ReadWideStringEntry(GsfInput* stm, std::string& entry);
uint32_t FourByteAlignedWideStringLength(const std::string& entry);
void AlignOutputAtFourBytes(GsfOutput* stm, uint32_t contentLength);
void AlignInputAtFourBytes(GsfInput* stm, uint32_t contentLength);
std::string ConvertCharStrToWideStr(const std::string& input);
std::string ConvertWideStrToCharStr(const std::string& input);

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_IRMUTILS_H
