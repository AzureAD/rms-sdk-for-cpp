/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <QFile>
#include <vector>
#include <CryptoAPI.h>
#include <BlockBasedProtectedStream.h>
#include "../PFile/PfileHeaderReader.h"
#include "../PFile/PfileHeaderWriter.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Core/ProtectionPolicy.h"
#include "../Platform/Logger/Logger.h"
#include "ProtectedFileStream.h"

using namespace rmscore::common;
using namespace rmscrypto::api;
using namespace rmscore::pfile;
using namespace std;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace modernapi {
GetProtectedFileStreamResult::GetProtectedFileStreamResult(
  GetUserPolicyResultStatus           status,
  std::shared_ptr<string>             referrer,
  std::shared_ptr<ProtectedFileStream>stream)
  : m_status(status)
  , m_referrer(referrer)
  , m_stream(stream)
{}

ProtectedFileStream::ProtectedFileStream(SharedStream          pImpl,
                                         shared_ptr<UserPolicy>policy,
                                         const string        & originalFileExtension)
  : m_policy(policy)
  , m_originalFileExtension(originalFileExtension)
  , m_pImpl(pImpl)
{}

ProtectedFileStream::~ProtectedFileStream() {}

shared_ptr<GetProtectedFileStreamResult>ProtectedFileStream::Acquire(
  SharedStream                       stream,
  const string                     & userId,
  IAuthenticationCallback          & authenticationCallback,
  IConsentCallback                  *consentCallback,
  PolicyAcquisitionOptions           options,
  ResponseCacheFlags                 cacheMask,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  Logger::Hidden("+ProtectedFileStream::Get");

  // Read PfileHeader from the stream
  shared_ptr<IPfileHeaderReader> headerReader = IPfileHeaderReader::Create();
  shared_ptr<PfileHeader> header              = nullptr;

  shared_ptr<UserPolicy> policy    = nullptr;
  GetUserPolicyResultStatus status = GetUserPolicyResultStatus::Success;
  shared_ptr<std::string>   referrer;
  PfileHeader *pHeader = nullptr;

  shared_ptr<ICryptoProvider> cp;

  try {
    header  = headerReader->Read(stream);
    pHeader = header.get();
    Logger::Hidden(
      "ProtectedFileStream: Read pfile header. Major version: %d, minor version: %d, file extension: '%s', content start position: %d, original file size: %I64d",
      pHeader->GetMajorVersion(),
      pHeader->GetMinorVersion(),
      pHeader->GetFileExtension().c_str(),
      pHeader->GetContentStartPosition(),
      pHeader->GetOriginalFileSize());
  } catch (exceptions::RMSException& e) {
    if ((e.error() != exceptions::RMSException::PFileError) ||
        (static_cast<exceptions::RMSPFileException&>(e).reason() !=
         exceptions::RMSPFileException::NotPFile))
    {
      throw;
    }
  }
  if (pHeader != nullptr) {
    ByteArray publishingLicense = pHeader->GetPublishingLicense();
    auto policyRequest          = UserPolicy::Acquire(publishingLicense,
                                                      userId,
                                                      authenticationCallback,
                                                      consentCallback,
                                                      options,
                                                      cacheMask,
                                                      cancelState);

    if ((policyRequest->Status == GetUserPolicyResultStatus::Success) &&
        (policyRequest->Policy != nullptr)) {
      // Successfully retrieved the policy
      policy = policyRequest->Policy;
    }
    status   = policyRequest->Status;
    referrer = policyRequest->Referrer;
  }

  ProtectedFileStream *protectedFileStream = policy ?
                                             CreateProtectedFileStream(policy,
                                                                       stream,
                                                                       header) :
                                             nullptr;

  auto result = make_shared<GetProtectedFileStreamResult>(
    move(status), referrer,
    shared_ptr<ProtectedFileStream>(protectedFileStream));
  return result;
}

shared_ptr<ProtectedFileStream>ProtectedFileStream::Create(
  shared_ptr<UserPolicy>policy,
  SharedStream          stream,
  const string        & originalFileExtension)
{
  Logger::Hidden("+ProtectedFileStream::Create");

  string ext = originalFileExtension.empty() ? ".pfile" : originalFileExtension;

  shared_ptr<PfileHeader> pHeader;

  if (policy.get() != nullptr)
  {
    auto headerWriter = IPfileHeaderWriter::Create();

    auto publishingLicense = policy->SerializedPolicy();
    ByteArray metadata; // No metadata

    // calculate content size
    uint32_t contentStartPosition =
      static_cast<uint32_t>(ext.size() +
                            publishingLicense.size()
                            +
                            metadata.size() + 454);
    pHeader = make_shared<PfileHeader>(move(publishingLicense),
                                       ext,
                                       contentStartPosition,
                                       static_cast<uint64_t>(-1), // No known
                                                                  // originalFileSize
                                       move(metadata),
                                       static_cast<uint32_t>(2),  // Major
                                                                  // version
                                       static_cast<uint32_t>(1),  // Minor
                                                                  // version
                                       CleartextRedirectHeader);

    headerWriter->Write(stream, pHeader);
    stream->Flush();
  }

  auto result = CreateProtectedFileStream(policy, stream, pHeader);

  Logger::Hidden("-ProtectedFileStream::Create");
  return shared_ptr<ProtectedFileStream>(result);
}

ProtectedFileStream * ProtectedFileStream::CreateProtectedFileStream(
  shared_ptr<UserPolicy>policy,
  SharedStream          stream,
  shared_ptr<rmscore::pfile::PfileHeader>
  header) {
  // create an IStreamImpl implementation of the backing stream
  auto pBackingStreamImpl            = stream->Clone();
  uint64_t nProtectedStreamBlockSize = 4096;

  shared_ptr<ICryptoProvider> pCryptoProvider = nullptr;
  ulong  contentStartPosition                 = 0;
  string fileExtension;

  // Disallow deprecated algorithms
  // There is no value in encrypting PFILEs in ECB mode, So treat the
  // deprecated flags for PFILEs as CBC4K mode
  // Check if deprecated flag is set, if yes - create a new crypto provider.
  if (policy->DoesUseDeprecatedAlgorithms()) {
    pCryptoProvider = CreateCryptoProvider(CipherMode::CIPHER_MODE_CBC4K,
                                           policy->GetImpl()->GetCryptoProvider()->GetKey());
  } else {
    pCryptoProvider = policy->GetImpl()->GetCryptoProvider();
  }

  // We want the cache block size to be 512 for cbc512, 4096 for cbc4k
  // In case of ECB blocksize is 16, Keep cache block size to be 4k.
  nProtectedStreamBlockSize = pCryptoProvider->GetBlockSize() == 512 ? 512 : 4096;

  // The protected stream block size must be a multiple of crypto provider's
  // block size,
  // otherwise the stream can't encrypt/decypt correctly.
  if (0 !=
      nProtectedStreamBlockSize %
      pCryptoProvider->GetBlockSize()) throw exceptions::RMSStreamException(
            "Invalid block size");

  contentStartPosition = header->GetContentStartPosition();
  fileExtension        = header->GetFileExtension();

  auto pProtectedStreamImpl = BlockBasedProtectedStream::Create(pCryptoProvider,
                                                                pBackingStreamImpl,
                                                                contentStartPosition,
                                                                stream->Size() -
                                                                contentStartPosition,
                                                                nProtectedStreamBlockSize);

  return new ProtectedFileStream(pProtectedStreamImpl, policy,
                                 fileExtension);
}

shared_future<int64_t>ProtectedFileStream::ReadAsync(uint8_t    *pbBuffer,
                                                     int64_t     cbBuffer,
                                                     int64_t     cbOffset,
                                                     std::launch launchType)
{
  return m_pImpl->ReadAsync(pbBuffer, cbBuffer, cbOffset, launchType);
}

shared_future<int64_t>ProtectedFileStream::WriteAsync(const uint8_t *cpbBuffer,
                                                      int64_t        cbBuffer,
                                                      int64_t        cbOffset,
                                                      std::launch    launchType)
{
  return m_pImpl->WriteAsync(cpbBuffer, cbBuffer, cbOffset, launchType);
}

future<bool>ProtectedFileStream::FlushAsync(std::launch launchType) {
  return m_pImpl->FlushAsync(launchType);
}

int64_t ProtectedFileStream::Read(uint8_t *pbBuffer,
                                  int64_t  cbBuffer)
{
  return m_pImpl->Read(pbBuffer, cbBuffer);
}

int64_t ProtectedFileStream::Write(const uint8_t *cpbBuffer,
                                   int64_t        cbBuffer)
{
  return m_pImpl->Write(cpbBuffer, cbBuffer);
}

bool ProtectedFileStream::Flush() {
  return m_pImpl->Flush();
}

SharedStream ProtectedFileStream::Clone()
{
  return shared_ptr<IStream>(new ProtectedFileStream(m_pImpl->Clone(), m_policy,
                                                     m_originalFileExtension));
}

void ProtectedFileStream::Seek(uint64_t u64Position)
{
  m_pImpl->Seek(u64Position);
}

uint64_t ProtectedFileStream::Position()
{
  return m_pImpl->Position();
}

bool ProtectedFileStream::CanRead() const
{
  return m_pImpl->CanRead();
}

bool ProtectedFileStream::CanWrite() const
{
  return m_pImpl->CanWrite();
}

uint64_t ProtectedFileStream::Size()
{
  return m_pImpl->Size();
}

void ProtectedFileStream::Size(uint64_t u64Value)
{
  m_pImpl->Size(u64Value);
}
} // namespace stream
} // namespace rmscore
