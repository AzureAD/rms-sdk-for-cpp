/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_OFFICE_PROTECTOR_METROOFFICEPROTECTOR_H
#define RMS_SDK_OFFICE_PROTECTOR_METROOFFICEPROTECTOR_H

#include "protector.h"
#include <iostream>
#include <memory>
#include <string>
#include <gsf/gsf.h>
#include "BlockBasedProtectedStream.h"
#include "file_api_structures.h"
#include "UserPolicy.h"

using namespace rmscore::fileapi;

namespace rmscore {
namespace officeprotector {

/*!
 * \brief The MetroOfficeProtector class Provides functionalities for protecting and unprotecting
 * office files in the new format (.docx, .xlsx, .pptx, etc)
 */
class MetroOfficeProtector : public Protector {
public:
  MetroOfficeProtector(std::string fileName, std::shared_ptr<std::istream> inputStream);

  ~MetroOfficeProtector();

  void ProtectWithTemplate(
      const UserContext& userContext,
      const ProtectWithTemplateOptions& options,
      std::shared_ptr<std::ostream> outputStream,
      std::shared_ptr<std::atomic<bool>> cancelState) override;

  void ProtectWithCustomRights(
      const UserContext& userContext,
      const ProtectWithCustomRightsOptions& options,
      std::shared_ptr<std::ostream> outputStream,
      std::shared_ptr<std::atomic<bool>> cancelState) override;

  UnprotectResult Unprotect(
      const UserContext& userContext,
      const UnprotectOptions& options,
      std::shared_ptr<std::ostream> outputStream,
      std::shared_ptr<std::atomic<bool>> cancelState) override;

  bool IsProtected() const override;

private:
  /*!
   * \brief Creates a Compound File on the temporary file. Writes the publishing license and other
   * components of DataSpaces storage. Then encrypts the input stream in chunks and writes to a
   * stream created in the Compound File.
   * \param outputTempFileName
   * \param inputFileSize
   */
  void ProtectInternal(std::string outputTempFileName, uint64_t inputFileSize);

  /*!
   * \brief Opens the input stream as a Compound File, reads publishing license and acquires User
   * Policy. Then decrypts the encrypted data present in the Compound File and writes to the
   * output stream.
   * \param userContext
   * \param options
   * \param outputStream
   * \param inputTempFileName
   * \param inputFileSize
   * \param cancelState
   * \return The result of the Unprotect operation as an enum value
   */
  UnprotectResult UnprotectInternal(
      const UserContext& userContext,
      const UnprotectOptions& options,
      std::shared_ptr<std::ostream> outputStream,
      std::string inputTempFileName,
      uint64_t inputFileSize,
      std::shared_ptr<std::atomic<bool>> cancelState);

  /*!
   * \brief Encrypts data in chunks of 4K bytes.
   * \param metroStream The output stream to which encrypted data is written.
   * \param inputFileSize The size of the input stream.
   */
  void EncryptStream(GsfOutput* metroStream, uint64_t inputFileSize);

  /*!
   * \brief Decrypts data in chunks of 4K bytes.
   * \param stdStream The output stream to which the decrypted data is written.
   * \param metroStream The input stream which is encrypted.
   * \param originalFileSize The size of the decrypted data found in the stream header.
   */
  void DecryptStream(
      const std::shared_ptr<std::ostream>& stdStream,
      GsfInput* metroStream,
      uint64_t originalFileSize);

  std::string mFileName;
  std::shared_ptr<std::istream> mInputStream;
  uint32_t mBlockSize;
  std::shared_ptr<modernapi::UserPolicy> mUserPolicy;
};

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_METROOFFICEPROTECTOR_H

