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

struct GsfOutput_deleter
{
    void operator () (GsfOutput* obj) const
    {
        gsf_output_close(obj);
        g_object_unref(G_OBJECT(obj));
    }
};

struct GsfOutfile_deleter
{
    void operator () (GsfOutfile* obj) const
    {
        gsf_output_close(reinterpret_cast<GsfOutput*>(obj));
        g_object_unref(G_OBJECT(obj));
    }
};

struct GsfInput_deleter
{
    void operator () (GsfInput* obj) const
    {
        g_object_unref(G_OBJECT(obj));
    }
};

struct GsfInfile_deleter
{
    void operator () (GsfInfile* obj) const
    {
        g_object_unref(G_OBJECT(obj));
    }
};

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_IRMUTILS_H
