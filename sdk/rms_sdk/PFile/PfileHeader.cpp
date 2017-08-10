/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "PfileHeader.h"

using namespace std;
using namespace rmscore::common;

namespace rmscore {
namespace pfile {
PfileHeader::PfileHeader(vector<uint8_t>  && publishingLicense,
                         const string& fileExtension,
                         uint32_t      contentStartPosition,
                         uint64_t      originalFileSize,
                         vector<uint8_t>  && metadata,
                         uint32_t      majorVersion,
                         uint32_t      minorVersion,
                         const string& cleartextRedirectionHeader) :
  m_PublishingLicense(publishingLicense),
  m_FileExtension(fileExtension), m_ContentStartPosition(contentStartPosition),
  m_OriginalFileSize(originalFileSize), m_Metadata(metadata), m_MajorVersion(
    majorVersion), m_MinorVersion(minorVersion), m_CleartextRedirectionHeader(
    cleartextRedirectionHeader) {}

const vector<uint8_t>& PfileHeader::GetPublishingLicense() const {
  return m_PublishingLicense;
}

const vector<uint8_t>& PfileHeader::GetMetadata() const {
  return m_Metadata;
}

const string& PfileHeader::GetFileExtension() const {
  return m_FileExtension;
}

uint32_t PfileHeader::GetContentStartPosition()  const {
  return m_ContentStartPosition;
}

uint64_t PfileHeader::GetOriginalFileSize()  const {
  return m_OriginalFileSize;
}

uint32_t PfileHeader::GetMajorVersion()  const {
  return m_MajorVersion;
}

uint32_t PfileHeader::GetMinorVersion()  const {
  return m_MinorVersion;
}

const string& PfileHeader::GetCleartextRedirectionHeader() const {
  return m_CleartextRedirectionHeader;
}
} // namespace pfile
} // namespace rmscore
