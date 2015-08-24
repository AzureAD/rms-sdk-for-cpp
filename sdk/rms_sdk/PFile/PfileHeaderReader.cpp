/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <future>
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Logger/Logger.h"
#include "PfileHeaderReader.h"

using namespace std;
using namespace rmscore::common;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace pfile {
const ByteArray ExpectedPreamble = { 0x2E, 0x70, 0x66, 0x69, 0x6C, 0x65 }; // byte
                                                                           // representation
                                                                           // of
                                                                           // ".pfile"

PfileHeaderReader::~PfileHeaderReader()
{}

shared_ptr<PfileHeader>PfileHeaderReader::Read(rmscrypto::api::SharedStream stream)
{
  Logger::Hidden("PfileHeaderReader: Reading pfile header.");

  CheckPreamble(stream);
  auto version                    = ReadVersionNumber(stream);
  auto cleartextRedirectionHeader = ReadCleartextRedirectionHeader(stream);

  return ReadHeader(stream, std::get<0>(version), std::get<1>(
                      version), cleartextRedirectionHeader);
}

void PfileHeaderReader::CheckPreamble(rmscrypto::api::SharedStream stream)
{
  ByteArray pr;

  Logger::Hidden("PfileHeaderReader: Checking preamble");

  auto expectedLength = static_cast<uint32_t>(ExpectedPreamble.size());
  ReadBytes(pr, stream, expectedLength);

  if (pr.size() != expectedLength)
  {
    throw exceptions::RMSPFileException("Invalid pfile preambule",
                                        exceptions::RMSPFileException::NotPFile);
  }

  for (uint32_t i = 0; i < pr.size(); ++i)
  {
    if (pr[i] != ExpectedPreamble[i]) {
      throw exceptions::RMSPFileException("Invalid pfile preambule",
                                          exceptions::RMSPFileException::NotPFile);
    }
  }
}

tuple<uint32_t, uint32_t>PfileHeaderReader::ReadVersionNumber(
  rmscrypto::api::SharedStream stream)
{
  const uint32_t MaxValidVersionNumber = 256;
  uint32_t majorVersion, minorVersion;

  stream->Read(reinterpret_cast<uint8_t *>(&majorVersion), sizeof(uint32_t));
  stream->Read(reinterpret_cast<uint8_t *>(&minorVersion), sizeof(uint32_t));

  Logger::Hidden("PfileHeaderReader: Major version: %d, Minor version: %d", majorVersion, minorVersion);

  if ((majorVersion >= MaxValidVersionNumber) ||
      (minorVersion >= MaxValidVersionNumber)) {
    throw exceptions::RMSPFileException("Invalid pfile version",
                                        exceptions::RMSPFileException::NotPFile);
  }

  if (majorVersion != 2) {
    throw exceptions::RMSPFileException("This version is not supported",
                                        exceptions::RMSPFileException::NotSupportedVersion);
  }

  return make_tuple(majorVersion, minorVersion);
}

string PfileHeaderReader::ReadCleartextRedirectionHeader(
  rmscrypto::api::SharedStream stream)
{
  ByteArray redirectHeader;
  uint32_t  redirectHeaderLength;

  stream->Read(reinterpret_cast<uint8_t *>(&redirectHeaderLength),
               sizeof(uint32_t));
  ReadBytes(redirectHeader, stream, redirectHeaderLength);

  if (redirectHeader.size() != redirectHeaderLength) {
    throw exceptions::RMSPFileException("Bad redirect header",
                                        exceptions::RMSPFileException::BadArguments);
  }

  string redirectHeaderStr(redirectHeader.begin(), redirectHeader.end());
  Logger::Hidden("PfileHeaderReader: Cleartext redirect header: %s", redirectHeaderStr.c_str());

  return redirectHeaderStr;
}

string PfileHeaderReader::ReadExtension(rmscrypto::api::SharedStream stream,
                                        uint32_t                     offset,
                                        uint32_t                     length)
{
  ByteArray ext;

  if (offset >= stream->Size()) {
    throw exceptions::RMSPFileException("Bad extension",
                                        exceptions::RMSPFileException::BadArguments);
  }

  ReadAtOffset(ext, stream, offset, length);
  string extStr(ext.begin(), ext.end());

  Logger::Hidden("PfileHeaderReader: Extension: %s", extStr.c_str());

  return extStr;
}

shared_ptr<PfileHeader>PfileHeaderReader::ReadHeader(
  rmscrypto::api::SharedStream stream,
  uint32_t                     majorVersion,
  uint32_t                     minorVersion,
  const string               & cleartextRedirectionHeader)
{
  uint32_t headerSize;
  uint32_t extensionOffset = 0;
  uint32_t extensionLength = 0;
  uint32_t plOffset        = 0;
  uint32_t plLength        = 0;
  uint32_t contentOffset   = 0;

  stream->Read(reinterpret_cast<uint8_t *>(&headerSize),      sizeof(uint32_t));
  stream->Read(reinterpret_cast<uint8_t *>(&extensionOffset), sizeof(uint32_t));
  stream->Read(reinterpret_cast<uint8_t *>(&extensionLength), sizeof(uint32_t));
  stream->Read(reinterpret_cast<uint8_t *>(&plOffset),        sizeof(uint32_t));
  stream->Read(reinterpret_cast<uint8_t *>(&plLength),        sizeof(uint32_t));
  stream->Read(reinterpret_cast<uint8_t *>(&contentOffset),   sizeof(uint32_t));

  uint32_t endOfHeader      = plOffset + plLength;
  uint64_t originalFileSize = 0;
  uint32_t metadataOffset   = 0;
  uint32_t metadataLength   = 0;

  ByteArray metadata;
  ByteArray publishingLicense;

  if ((majorVersion >= 2) && (minorVersion >= 1))
  {
    stream->Read(reinterpret_cast<uint8_t *>(&originalFileSize),
                 sizeof(uint64_t));
    stream->Read(reinterpret_cast<uint8_t *>(&metadataOffset),
                 sizeof(uint32_t));
    stream->Read(reinterpret_cast<uint8_t *>(&metadataLength),
                 sizeof(uint32_t));
    endOfHeader = metadataOffset + metadataLength;
  }

  if (contentOffset < endOfHeader) {
    throw exceptions::RMSPFileException("Bad content offset",
                                        exceptions::RMSPFileException::BadArguments);
  }

  string extension = ReadExtension(stream, extensionOffset, extensionLength);

  ReadAtOffset(publishingLicense, stream, plOffset, plLength);

  // test if publishingLicense is UTF-16
  if ((publishingLicense.size() > 10) && (publishingLicense[0] == '\0' || publishingLicense[1] == '\0')) {
      publishingLicense.push_back(0);
      auto strUnicode = QString::fromUtf16((const ushort*)&publishingLicense[publishingLicense[0] == '\0' ? 1 : 0],
                                           static_cast<int>(publishingLicense.size() / sizeof(ushort)));
      auto str = strUnicode.toUtf8();

      publishingLicense = ByteArray(str.begin(), str.end());

  } else
  // remove UTF-8 BOM
  if ((publishingLicense.size() > 3) &&
      (memcmp(publishingLicense.data(), "\xEF\xBB\xBF", 3) == 0)) {
    publishingLicense.erase(publishingLicense.begin(),
                            publishingLicense.begin() + 3);
  }

  if ((majorVersion == 2) && (minorVersion == 1))
  {
    ReadAtOffset(metadata, stream, metadataOffset, metadataLength);
  }
  return make_shared<PfileHeader>(move(publishingLicense), extension,
                                  contentOffset, originalFileSize,
                                  move(metadata), majorVersion, minorVersion,
                                  cleartextRedirectionHeader);
}

void PfileHeaderReader::ReadAtOffset(ByteArray                  & dst,
                                     rmscrypto::api::SharedStream stream,
                                     uint32_t                     offset,
                                     uint32_t                     length)
{
  stream->Seek(offset);
  ReadBytes(dst, stream, length);
}

void PfileHeaderReader::ReadBytes(ByteArray                  & dst,
                                  rmscrypto::api::SharedStream stream,
                                  uint32_t                     length)
{
  // check for size
  if (length == 0) return;

  if (length > stream->Size()) {
    throw exceptions::RMSPFileException("Bad block length",
                                        exceptions::RMSPFileException::BadArguments);
  }

  auto pos = dst.size();
  dst.resize(pos + length);

  stream->Read((uint8_t *)&dst[pos], length);
}

shared_ptr<IPfileHeaderReader>IPfileHeaderReader::Create()
{
  return std::dynamic_pointer_cast<IPfileHeaderReader, PfileHeaderReader>(
    std::make_shared<PfileHeaderReader>());
}
} // namespace pfile
} // namespace rmscore
