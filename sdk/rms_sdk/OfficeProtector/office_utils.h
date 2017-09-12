/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef RMS_SDK_OFFICE_PROTECTOR_OFFICEUTILS_H
#define RMS_SDK_OFFICE_PROTECTOR_OFFICEUTILS_H

#include <string>
#include <gsf/gsf.h>
#include "BlockBasedProtectedStream.h"
#include "file_api_structures.h"
#include "UserPolicy.h"

namespace rmscore {
namespace officeutils {

using namespace rmscore::fileapi;

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
 * \brief Aligns the stream pointed to by 'stm' to 4 byte length by writing null characters to the
 * stream.
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
 * \param utf16_string the input string in UTF16 encoding
 * \return The UTF8 encoded string
 */
std::string utf16_to_utf8(const std::u16string& utf16_string);

/*!
 * \brief Converts a UTF8 encoded string to a UTF16 encoded string
 * \param utf16_string The input string in UTF8 encoding
 * \return UTF16 encoded string
 */
std::u16string utf8_to_utf16(const std::string& utf8_string);

/*!
 * \brief Writes the header of the encryped stream. Currently just contains the length of the
 * unencrypted data.
 * \param stm
 * \param contentLength
 */
void WriteStreamHeader(GsfOutput* stm, const uint64_t& contentLength);

/*!
 * \brief Reads the header of the encryped stream. Currently just contains the length of the
 * unencrypted data.
 * \param stm
 * \param contentLength
 */
void ReadStreamHeader(GsfInput* stm, uint64_t& contentLength);

/*!
 * \brief Uses 'cryptoOptions' and 'allowAuditedExtraction' params to create enum value
 * \param allowAuditedExtraction
 * \param cryptoOptions
 * \return An enum which has the required user policy creation options
 */
modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptionsForOffice(
    const bool& allowAuditedExtraction,
    CryptoOptions cryptoOptions);

/*!
 * \brief CreateProtectedStream
 * \param stream The Shared Stream which is wrapped into a protected stream
 * \param streamSize
 * \param cryptoProvider
 * \return A shared pointer to the protected stream.
 */
std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
    const rmscrypto::api::SharedStream& stream,
    uint64_t streamSize,
    std::shared_ptr<rmscrypto::api::ICryptoProvider> cryptoProvider);

/*!
 * \brief Tries to open the input stream as a Compound File and read the publishing license. If the
 * license is successfully read, the file is protected.
 * \param inputTempFileName
 * \param inputFileSize
 * \return
 */
bool IsProtectedInternal(
    std::istream* inputStream,
    std::string inputTempFileName,
    uint64_t inputFileSize,
    bool isMetro);

/*!
 * \brief Reads 'length' number of bytes from GsfInput stream 'stm' into the 'buffer' and checks for errors.
 * \param stm
 * \param length
 * \param buffer
 */
void GsfRead(GsfInput* stm, uint32_t length, uint8_t* buffer);

/*!
 * \brief Writes 'length' number of bytes from 'buffer' into the GsfOutput stream 'stm' and checks for errors.
 * \param stm
 * \param length
 * \param buffer
 */
void GsfWrite(GsfOutput* stm, uint32_t length, const uint8_t* buffer);

/*!
 * \brief Seeks to 'offset' from 'position' in the GsfInput stream 'stm' and checks for errors.
 * \param stm
 * \param offset
 * \param position
 */
void GsfInputSeek(GsfInput* stm, uint64_t offset, GSeekType position);

/*!
 * \brief Seeks to 'offset' from 'position' in the GsfOutput stream 'stm' and checks for errors.
 * \param stm
 * \param offset
 * \param position
 */
void GsfOutputSeek(GsfOutput* stm, uint64_t offset, GSeekType position);

/*!
 * \brief Checks whether the GsfInput stream 'stm' is null or not.
 * \param stm
 */
void CheckGsfInput(GsfInput* stm);

/*!
 * \brief Checks whether the GsfOutput stream 'stm' is null or not.
 * \param stm
 */
void CheckGsfOutput(GsfOutput* stm);

/*!
 * \brief Checks whether the GsfInfile storage 'stg' is null or not.
 * \param stg
 */
void CheckGsfInfile(GsfInfile* stg);

/*!
 * \brief Checks whether the GsfOutfile storage 'stg' is null or not.
 * \param stg
 */
void CheckGsfOutfile(GsfOutfile* stg);

} // namespace officeutils
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_OFFICEUTILS_H
