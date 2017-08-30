/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_PFILE_PFILEPROTECTOR_H
#define RMS_SDK_PFILE_PFILEPROTECTOR_H

#include <iostream>
#include <memory>
#include <string>
#include "BlockBasedProtectedStream.h"
#include "file_api_structures.h"
#include "protector.h"
#include "UserPolicy.h"
#include "PfileHeader.h"

using namespace rmscore::fileapi;

namespace rmscore {
namespace pfile {

class PFileProtector : public Protector {
public:
  PFileProtector(const std::string& originalFileExtension, std::shared_ptr<std::istream> inputStream);

  ~PFileProtector();

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
   * \brief Creates protected stream from output stream. Writes the header, protects and writes the
   * content as binary from the input stream.
   * \param outputStream
   * \param inputFileSize Size of the input stream.
   */
  void ProtectInternal(const std::shared_ptr<std::ostream>& outputStream, uint64_t inputFileSize);

  /*!
   * \brief Wraps a rmscrypto::api::SharedStream into a protected stream. The 'Write' method for this
   * stream encrypts the data and then writes to the underlying stream. The 'Read' method decrypts the
   * data from the underlying stream.
   * \param stream
   * \param header
   * \return A shared pointer to the protected stream
   */
  std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
      const rmscrypto::api::SharedStream& stream,
      const std::shared_ptr<pfile::PfileHeader>& header);

  /*!
   * \brief Protects data from input stream in chunks and writes to 'pStream'. This stream encrypts
   * data and then writes to the underlying stream.
   * \param pStream
   * \param inputFileSize
   */
  void EncryptStream(
      const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
      uint64_t inputFileSize);

  /*!
   * \brief Reads data from 'pStream' in chunks and writes to the 'stdStream'. The 'Read' method
   * for 'pStream' decrypts the data and then returns it.
   * \param stdStream
   * \param pStream
   * \param originalFileSize
   */
  void DecryptStream(
      const std::shared_ptr<std::ostream>& stdStream,
      const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
      uint64_t originalFileSize);

  /*!
   * \brief Creates and writes the PFile header
   * \param stream
   * \param originalFileSize
   * \return A shared pointer to the PFileHeader object created and written
   */
  std::shared_ptr<rmscore::pfile::PfileHeader> WriteHeader(
      const rmscrypto::api::SharedStream& stream,
      uint64_t originalFileSize);

  /*!
   * \brief Reads and returns the PFile header
   * \param stream
   * \return A shared pointer to a PFileHeader object created after reading the header from stream
   */
  std::shared_ptr<rmscore::pfile::PfileHeader> ReadHeader(
      const rmscrypto::api::SharedStream& stream) const;

  std::string mOriginalFileExtension;
  std::shared_ptr<std::istream> mInputStream;
  uint32_t mBlockSize;
  std::shared_ptr<modernapi::UserPolicy> mUserPolicy;
};

} // namespace pfile
} // namespace rmscore

#endif // RMS_SDK_PFILE_PFILEPROTECTOR_H

