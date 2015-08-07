/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <future>
#include "PfileHeaderWriter.h"
#include "PfileHeader.h"
#include "../Platform/Logger/Logger.h"

using namespace std;
using namespace rmscore::common;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace pfile {
PfileHeaderWriter::~PfileHeaderWriter()
{}

size_t PfileHeaderWriter::Write(rmscrypto::api::SharedStream      stream,
                                const std::shared_ptr<PfileHeader>header)
{
  Logger::Hidden("PfileHeaderWriter::Write");

  auto extension = header->GetFileExtension();
  Logger::Hidden("Writing pfile header. Major version: %d, minor version: %d," \
                 " file extension: %s, content start position: %d, original file size: %I64d",
                 header->GetMajorVersion(),
                 header->GetMinorVersion(),
                 extension.c_str(),
                 header->GetContentStartPosition(),
                 header->GetOriginalFileSize());

  uint32_t position = WritePreamble(stream);

  position += WriteVersionNumber(stream, header);
  position += WriteCleartextRedirection(stream, header);

  WriteHeader(stream, header, position);
  WriteExtension(stream, header);
  WritePublishingLicense(stream, header);
  WriteMetadata(stream, header);

  return stream->Size();
}

uint32_t PfileHeaderWriter::WritePreamble(rmscrypto::api::SharedStream writer)
{
  Logger::Hidden("PfileHeaderWriter::WritePreamble");
  common::ByteArray preamble = { 0x2E, 0x70, 0x66, 0x69, 0x6C, 0x65 }; // byte

  // representation
  // of
  // ".pfile"
  writer->Write(reinterpret_cast<const uint8_t *>(preamble.data()),
                static_cast<int>(preamble.size()));

  return static_cast<uint32_t>(preamble.size());
}

uint32_t PfileHeaderWriter::WriteVersionNumber(
  rmscrypto::api::SharedStream      writer,
  const std::shared_ptr<PfileHeader>header)
{
  Logger::Hidden("PfileHeaderWriter::WriteVersionNumber");
  uint32_t mjv = header->GetMajorVersion();
  uint32_t mnv = header->GetMinorVersion();

  writer->Write(reinterpret_cast<uint8_t *>(&mjv), sizeof(uint32_t));
  writer->Write(reinterpret_cast<uint8_t *>(&mnv), sizeof(uint32_t));

  return 2 * sizeof(uint32_t);
}

uint32_t PfileHeaderWriter::WriteCleartextRedirection(
  rmscrypto::api::SharedStream      writer,
  const std::shared_ptr<PfileHeader>header)
{
  Logger::Hidden("PfileHeaderWriter::WriteCleartextRedirection");
  auto cleartextRedirectionHeader = header->GetCleartextRedirectionHeader();
  uint32_t cts                    =
    static_cast<uint32_t>(cleartextRedirectionHeader.size());

  writer->Write(reinterpret_cast<const uint8_t *>(&cts), sizeof(uint32_t));
  writer->Write(reinterpret_cast<const uint8_t *>(cleartextRedirectionHeader.
                                                  data()),
                static_cast<int>(cleartextRedirectionHeader.size()));

  return static_cast<uint32_t>(cleartextRedirectionHeader.size()) +
         sizeof(uint32_t);
}

void PfileHeaderWriter::WriteHeader(rmscrypto::api::SharedStream      writer,
                                    const std::shared_ptr<PfileHeader>header,
                                    size_t                            headerOffset)
{
  Logger::Hidden("PfileHeaderWriter::WriteHeader");
  uint32_t headerSize      = 8 * sizeof(uint32_t) + sizeof(uint64_t);
  uint32_t extensionOffset = static_cast<uint32_t>(headerOffset) +
                             headerSize;
  uint32_t extensionLength =
    static_cast<uint32_t>(header->GetFileExtension().size());
  uint32_t plOffset = extensionOffset + extensionLength;
  uint32_t plLength =
    static_cast<uint32_t>(header->GetPublishingLicense().size());
  uint64_t originalFileSize = header->GetOriginalFileSize();
  uint32_t metadataOffset   = plOffset + plLength;
  uint32_t metadataLength   = static_cast<uint32_t>(header->GetMetadata().size());
  uint32_t contentOffset    = metadataOffset + metadataLength;

  writer->Write(reinterpret_cast<uint8_t *>(&headerSize),       sizeof(uint32_t));
  writer->Write(reinterpret_cast<uint8_t *>(&extensionOffset),  sizeof(uint32_t));
  writer->Write(reinterpret_cast<uint8_t *>(&extensionLength),  sizeof(uint32_t));
  writer->Write(reinterpret_cast<uint8_t *>(&plOffset),         sizeof(uint32_t));
  writer->Write(reinterpret_cast<uint8_t *>(&plLength),         sizeof(uint32_t));
  writer->Write(reinterpret_cast<uint8_t *>(&contentOffset),    sizeof(uint32_t));
  writer->Write(reinterpret_cast<uint8_t *>(&originalFileSize), sizeof(uint64_t));
  writer->Write(reinterpret_cast<uint8_t *>(&metadataOffset),   sizeof(uint32_t));
  writer->Write(reinterpret_cast<uint8_t *>(&metadataLength),   sizeof(uint32_t));
}

void PfileHeaderWriter::WriteExtension(rmscrypto::api::SharedStream      writer,
                                       const std::shared_ptr<PfileHeader>header)
{
  Logger::Hidden("PfileHeaderWriter::WriteExtension");
  auto extension = header->GetFileExtension();

  if (extension.empty()) return;

  writer->Write(reinterpret_cast<const uint8_t *>(extension.data()),
                static_cast<int>(extension.length()));
}

void PfileHeaderWriter::WritePublishingLicense(
  rmscrypto::api::SharedStream      writer,
  const std::shared_ptr<PfileHeader>header)
{
  Logger::Hidden("PfileHeaderWriter::WritePublishingLicense");
  auto publishingLicense = header->GetPublishingLicense();

  writer->Write(reinterpret_cast<const uint8_t *>(publishingLicense.data()),
                static_cast<int>(publishingLicense.size()));
}

void PfileHeaderWriter::WriteMetadata(rmscrypto::api::SharedStream      writer,
                                      const std::shared_ptr<PfileHeader>header)
{
  Logger::Hidden("PfileHeaderWriter::WriteMetadata");
  auto metadata = header->GetMetadata();

  writer->Write(reinterpret_cast<const uint8_t *>(metadata.data()),
                static_cast<int>(metadata.size()));
}

shared_ptr<IPfileHeaderWriter>IPfileHeaderWriter::Create()
{
  Logger::Hidden("PfileHeaderWriter::Create");

  return make_shared<PfileHeaderWriter>();
}
} // namespace pfile
} // namespace rmscore
