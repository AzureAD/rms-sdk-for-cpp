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
  void ProtectInternal(
      const std::string& inputTempFileName,
      const std::string& outputTempFileName,
      const std::string& drmTempFileName,
      uint64_t inputFileSize);

  UnprotectResult UnprotectInternal(
      const UserContext& userContext,
      const UnprotectOptions& options,
      const std::string& inputTempFileName,
      const std::string& outputTempFileName,
      const std::string& drmTempFileName,
      uint64_t inputFileSize,
      std::shared_ptr<std::atomic<bool>> cancelState);

  bool IsProtectedInternal(const std::string& inputTempFileName, uint64_t inputFileSize) const;

  std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
      const rmscrypto::api::SharedStream& stream,
      uint64_t streamSize,
      std::shared_ptr<rmscrypto::api::ICryptoProvider> cryptoProvider);

  void EncryptStream(FILE* drmStream, GsfOutput* drmEncryptedStream, uint64_t inputFileSize);

  void DecryptStream(FILE* drmStream, GsfInput* drmEncryptedStream, uint64_t originalFileSize);

  bool CopyStorage(GsfInfile* src, GsfOutfile* dest);

  bool CopyStream(GsfInput* src, GsfOutput* dest);

  void CopyTemplate(GsfOutfile* dest, uint32_t identifier);

  const std::vector<std::string>& GetStorageElementsList();

  const std::map<std::string, uint32_t>& GetIdentifierMap();

  std::string mFileName;
  std::shared_ptr<std::istream> mInputStream;
  uint32_t mBlockSize;
  std::shared_ptr<modernapi::UserPolicy> mUserPolicy;
};

} // namespace officeprotector
} // namespace rmscore
#endif // RMS_SDK_FILE_API_MSOOFFICEPROTECTOR_H
