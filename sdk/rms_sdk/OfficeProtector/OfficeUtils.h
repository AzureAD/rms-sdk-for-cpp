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
namespace officeutils {

/*!
 * \brief Converts 'entry' to UTF16 and writes it to the stream pointed by 'stm'. After that,
 * aligns the stream to 4 byte length.
 * \param stm
 * \param entry
 */
void WriteWideStringEntry(GsfOutput* stm, const std::string& entry);

/*!
 * \brief Reads a UTF16 string, converts it to UTF8 and assigns the value to 'entry'. After that,
 * aligns the stream to 4 byte length.
 * \param stm
 * \param entry
 */
void ReadWideStringEntry(GsfInput* stm, std::string& entry);

/*!
 * \brief Rounds up the length of 'entry' in bytes to the nearest 4 bytes using a bitmask
 * \param entry
 * \return
 */
uint32_t FourByteAlignedWideStringLength(const std::string& entry);

/*!
 * \brief Aligns the stream pointed to by 'stm' to 4 byte length by writing null characters to the stream.
 * \param stm
 * \param contentLength
 */
void AlignOutputAtFourBytes(GsfOutput* stm, uint32_t contentLength);

/*!
 * \brief Aligns the stream pointed to by 'stm' to 4 byte length by seeking ahead.
 * \param stm
 * \param contentLength
 */
void AlignInputAtFourBytes(GsfInput* stm, uint32_t contentLength);

/*!
 * \brief Converts a UTF16 encoded string to a UTF8 encoded string
 * \param utf16_string
 * \return
 */
std::string utf16_to_utf8(const std::u16string& utf16_string);

/*!
 * \brief Converts a UTF8 encoded string to a UTF16 encoded string
 * \param utf16_string
 * \return
 */
std::u16string utf8_to_utf16(const std::string& utf8_string);

/*!
 * \brief Writes the header of the encryped stream. Currently just contains the length of the unencrypted data.
 * \param stm
 * \param contentLength
 */
void WriteStreamHeader(GsfOutput* stm, const uint64_t& contentLength);

/*!
 * \brief Reads the header of the encryped stream. Currently just contains the length of the unencrypted data.
 * \param stm
 * \param contentLength
 */
void ReadStreamHeader(GsfInput* stm, uint64_t& contentLength);

} // namespace officeutils
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_IRMUTILS_H
