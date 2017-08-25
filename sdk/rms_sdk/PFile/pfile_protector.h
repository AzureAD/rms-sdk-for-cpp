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
  void ProtectInternal(const std::shared_ptr<std::ostream>& outputStream, uint64_t inputFileSize);

  std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
      const rmscrypto::api::SharedStream& stream,
      const std::shared_ptr<pfile::PfileHeader>& header);

  void EncryptStream(
      const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
      uint64_t inputFileSize);

  void DecryptStream(
      const std::shared_ptr<std::ostream>& stdStream,
      const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
      uint64_t originalFileSize);

  std::shared_ptr<rmscore::pfile::PfileHeader> WriteHeader(
      const rmscrypto::api::SharedStream& stream,
      uint64_t originalFileSize);

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

