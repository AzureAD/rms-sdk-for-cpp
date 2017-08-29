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
   * \brief ProtectInternal
   *
   *
   * \param outputTempFileName
   * \param inputFileSize
   */
  void ProtectInternal(std::string outputTempFileName, uint64_t inputFileSize);

  UnprotectResult UnprotectInternal(
      const UserContext& userContext,
      const UnprotectOptions& options,
      std::shared_ptr<std::ostream> outputStream,
      std::string inputTempFileName,
      uint64_t inputFileSize,
      std::shared_ptr<std::atomic<bool>> cancelState);

  bool IsProtectedInternal(std::string inputTempFileName, uint64_t inputFileSize) const;


  std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
      const rmscrypto::api::SharedStream& stream,
      uint64_t streamSize,
      std::shared_ptr<rmscrypto::api::ICryptoProvider> cryptoProvider);

  void EncryptStream(GsfOutput* metroStream, uint64_t inputFileSize);

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

