/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_MSOOFFICEPROTECTOR_H
#define RMS_SDK_FILE_API_MSOOFFICEPROTECTOR_H

#include "protector.h"
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <gsf/gsf.h>
#include "BlockBasedProtectedStream.h"
#include "file_api_structures.h"
#include "UserPolicy.h"

using namespace rmscore::fileapi;

namespace rmscore {
namespace officeprotector {

class MsoOfficeProtector : public Protector {
public:
  MsoOfficeProtector(const std::string& fileName, std::shared_ptr<std::istream> inputStream);

  ~MsoOfficeProtector();

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
   * \brief Copies the input stream into a temporary file with name 'inputTempFileName' to open as
   * a Compound File. Creates Compound Files on the output file 'outputTempFileName' and temporary
   * files 'drmTempFileName'. Some part of the input file is copied as it is to the output Compound
   * File and some is copied to the drm Compound File. Then the the drm Compound File is encrypted
   * as binary and written to a stream inside output Compound File.
   * \param inputTempFileName
   * \param outputTempFileName
   * \param drmTempFileName
   * \param inputFileSize
   */
  void ProtectInternal(
      const std::string& inputTempFileName,
      const std::string& outputTempFileName,
      const std::string& drmTempFileName,
      uint64_t inputFileSize);

  /*!
   * \brief Copies the input stream into a temporary file with name 'inputTempFileName' to open as
   * a Compound File. Creates Compound File on the output file 'outputTempFileName'.Some part of the
   * input file which is not encrypted is copied as it is to the output Compound File.
   * The encrypted stream is decrypted and written to the temporary file 'drmTempFileName'. Then this
   * file is opened as a Compound File and all streams/storages are copied to the output Compound file.
   * \param userContext
   * \param options
   * \param inputTempFileName
   * \param outputTempFileName
   * \param drmTempFileName
   * \param inputFileSize
   * \param cancelState
   * \return
   */
  UnprotectResult UnprotectInternal(
      const UserContext& userContext,
      const UnprotectOptions& options,
      const std::string& inputTempFileName,
      const std::string& outputTempFileName,
      const std::string& drmTempFileName,
      uint64_t inputFileSize,
      std::shared_ptr<std::atomic<bool>> cancelState);

  /*!
   * \brief Encrypts data in chunks of 4K bytes.
   * \param drmStream The input stream which is encrypted as binary.
   * \param drmEncryptedStream The output stream to which encrypted data is written.
   * \param inputFileSize The size of the input stream.
   */
  void EncryptStream(FILE* drmStream, GsfOutput* drmEncryptedStream, uint64_t inputFileSize);

  /*!
   * \brief Decrypts data in chunks of 4K bytes.
   * \param drmStream The output stream to which the decrypted data is written.
   * \param drmEncryptedStream The input stream which is encrypted.
   * \param originalFileSize The size of the decrypted data found in the stream header.
   */
  void DecryptStream(FILE* drmStream, GsfInput* drmEncryptedStream, uint64_t originalFileSize);

  /*!
   * \brief Recursively copies all streams and storages in src to dest.
   * \param src
   * \param dest
   * \return
   */
  bool CopyStorage(GsfInfile* src, GsfOutfile* dest);

  /*!
   * \brief Copies all contents of src to dest.
   * \param src
   * \param dest
   * \return
   */
  bool CopyStream(GsfInput* src, GsfOutput* dest);

  /*!
   * \brief Creates streams to display an appropriate message in non enlightened applications.
   * \param dest The output file Compound File
   * \param identifier An index which identifies which type of office file and what streams need to copied.
   */
  void CopyTemplate(GsfOutfile* dest, uint32_t identifier);

  /*!
   * \brief Creates a vector of all storages/streams in the input which don't need to be
   * encrypted and can be copied to output as is.
   * \return
   */
  const std::vector<std::string>& GetStorageElementsList();

  /*!
   * \brief Creates a map where the key is a string which uniquely identifies whether it
   * is a Word, Powerpoint or Excel file. The value is just an index to be used in the calling function.
   * \return
   */
  const std::map<std::string, uint32_t>& GetIdentifierMap();

  std::string mFileName;
  std::shared_ptr<std::istream> mInputStream;
  uint32_t mBlockSize;
  std::shared_ptr<modernapi::UserPolicy> mUserPolicy;
};

} // namespace officeprotector
} // namespace rmscore
#endif // RMS_SDK_FILE_API_MSOOFFICEPROTECTOR_H
