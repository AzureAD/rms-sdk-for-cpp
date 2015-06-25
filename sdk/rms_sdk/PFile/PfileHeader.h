/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_PFILEHEADER_H_
#define _RMS_LIB_PFILEHEADER_H_

#include <string>
#include <IStream.h>
#include "../Common/CommonTypes.h"

namespace rmscore {
namespace pfile {
class PfileHeader {
public:

  PfileHeader(common::ByteArray&& publishingLicense,
              const std::string & fileExtension,
              uint32_t            contentStartPosition,
              uint64_t            originalFileSize,
              common::ByteArray&& metadata,
              uint32_t            majorVersion,
              uint32_t            minorVersion,
              const std::string & cleartextRedirectionHeader);

  const common::ByteArray& GetPublishingLicense() const;
  const common::ByteArray& GetMetadata() const;
  const std::string      & GetFileExtension() const;
  uint32_t                 GetContentStartPosition() const;
  uint64_t                 GetOriginalFileSize() const;
  uint32_t                 GetMajorVersion() const;
  uint32_t                 GetMinorVersion() const;
  const std::string      & GetCleartextRedirectionHeader() const;

private:

  common::ByteArray m_PublishingLicense;
  std::string m_FileExtension;
  const uint32_t m_ContentStartPosition;
  const uint64_t m_OriginalFileSize;
  common::ByteArray m_Metadata;
  const uint32_t    m_MajorVersion;
  const uint32_t    m_MinorVersion;
  std::string       m_CleartextRedirectionHeader;
};
} // namespace pfile
} // namespace rmscore
#endif // _RMS_LIB_PFILEHEADER_H_
