/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "../ModernAPI/RMSExceptions.h"
#include "../Core/ProtectionPolicy.h"
#include "../Platform/Logger/Logger.h"
#include <BlockBasedProtectedStream.h>
#include "CustomProtectedStream.h"

using namespace std;
using namespace rmscrypto::api;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace modernapi {
CustomProtectedStream::CustomProtectedStream(shared_ptr<IStream>pImpl)
  : m_pImpl(pImpl)
{}

CustomProtectedStream::~CustomProtectedStream()
{}

shared_ptr<CustomProtectedStream>CustomProtectedStream::Create(
  shared_ptr<UserPolicy>policy,
  SharedStream          stream,
  uint64_t              contentStartPosition,
  uint64_t              contentSize)
{
  Logger::Hidden("+CustomProtectedStream::Create");

  if (policy.get() == nullptr) {
    throw exceptions::RMSInvalidArgumentException("Invalid policy argument");
  }
  auto pCryptoProvider = policy->GetImpl()->GetCryptoProvider();

  // create an IStreamImpl implementation of the backing stream
  auto pBackingStreamImpl = stream;

  // We want the cache block size to be 512 for cbc512, 4096 for cbc4k
  // In case of ECB blocksize is 16, Keep cache block size to be 4k.
  uint64_t nProtectedStreamBlockSize = pCryptoProvider->GetBlockSize() ==
                                       512 ? 512 : 4096;

  // The protected stream block size must be a multiple of crypto
  // provider's block size,
  // otherwise the stream can't encrypt/decypt correctly.
  if (0 != nProtectedStreamBlockSize % pCryptoProvider->GetBlockSize()) {
    throw exceptions::RMSCryptographyException("Invalid block size");
  }

  auto pProtectedStreamImpl = BlockBasedProtectedStream::Create(pCryptoProvider,
                                                                pBackingStreamImpl,
                                                                contentStartPosition,
                                                                contentSize,
                                                                nProtectedStreamBlockSize);

  auto result =
    shared_ptr<CustomProtectedStream>(new CustomProtectedStream(
                                        pProtectedStreamImpl));

  Logger::Hidden("-CustomProtectedStream::Create");
  return result;
}

uint64_t CustomProtectedStream::GetEncryptedContentLength(
  std::shared_ptr<UserPolicy>policy,
  uint64_t                   contentLength)
{
  if (policy.get() == nullptr) {
    throw exceptions::RMSNullPointerException("Invalid policy argument");
  }
  return policy->GetImpl()->GetCryptoProvider()->GetCipherTextSize(
    contentLength);
}

shared_future<int64_t>CustomProtectedStream::ReadAsync(uint8_t    *pbBuffer,
                                                       int64_t     cbBuffer,
                                                       int64_t     cbOffset,
                                                       std::launch launchType)
{
  return m_pImpl->ReadAsync(pbBuffer, cbBuffer, cbOffset, launchType);
}

shared_future<int64_t>CustomProtectedStream::WriteAsync(const uint8_t *cpbBuffer,
                                                        int64_t        cbBuffer,
                                                        int64_t        cbOffset,
                                                        std::launch    launchType)
{
  return m_pImpl->WriteAsync(cpbBuffer, cbBuffer, cbOffset, launchType);
}

future<bool>CustomProtectedStream::FlushAsync(std::launch launchType) {
  return m_pImpl->FlushAsync(launchType);
}

int64_t CustomProtectedStream::Read(uint8_t *pbBuffer,
                                    int64_t  cbBuffer)
{
  return m_pImpl->Read(pbBuffer, cbBuffer);
}

int64_t CustomProtectedStream::Write(const uint8_t *cpbBuffer,
                                     int64_t        cbBuffer)
{
  return m_pImpl->Write(cpbBuffer, cbBuffer);
}

bool CustomProtectedStream::Flush() {
  return m_pImpl->Flush();
}

SharedStream CustomProtectedStream::Clone()
{
  return static_pointer_cast<IStream>(
    shared_ptr<CustomProtectedStream>(new CustomProtectedStream(m_pImpl->Clone())));
}

void CustomProtectedStream::Seek(uint64_t u64Position)
{
  m_pImpl->Seek(u64Position);
}

bool CustomProtectedStream::CanRead() const
{
  return m_pImpl->CanRead();
}

bool CustomProtectedStream::CanWrite() const
{
  return m_pImpl->CanWrite();
}

uint64_t CustomProtectedStream::Position()
{
  return m_pImpl->Position();
}

uint64_t CustomProtectedStream::Size()
{
  return m_pImpl->Size();
}

void CustomProtectedStream::Size(uint64_t u64Value)
{
  m_pImpl->Size(u64Value);
}

shared_ptr<IStream>CustomProtectedStream::GetImpl()
{
  return m_pImpl;
}
} // namespace modernapi
} // namespace rmscore
