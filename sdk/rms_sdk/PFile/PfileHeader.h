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

static const uint32_t MIN_SUPPORTED_MJVERSION_FOR_READING = 2;
static const uint32_t MAX_SUPPORTED_MJVERSION_FOR_READING = 3;
static const uint32_t MJVERSION_FOR_WRITING = 3;
static const uint32_t MNVERSION_FOR_WRITING = 0;
// PFiles from this version and below were forced to be written with CBC4K encryption, regardless
// of the published license information.
static const uint32_t MaxMajorVerionsCBC4KIsForced = 2;

class PfileHeader {
public:

  PfileHeader(common::ByteArray&& publishing_license,
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
