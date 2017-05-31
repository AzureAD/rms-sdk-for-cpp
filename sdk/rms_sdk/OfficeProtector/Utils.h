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
#include "pole.h"

namespace rmscore {
namespace officeprotector {

uint32_t WriteWideStringEntry(std::shared_ptr<pole::Stream> stm, const std::string& entry);
uint32_t ReadWideStringEntry(std::shared_ptr<pole::Stream> stm, std::string& entry);
uint32_t FourByteAlignedWideStringLength(const std::string& entry);
uint32_t AlignAtFourBytes(std::shared_ptr<pole::Stream> stm, uint32_t contentLength, bool write);
std::string ConvertCharStrToWideStr(const std::string& input);
std::string ConvertWideStrToCharStr(const std::string& input);

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_IRMUTILS_H
