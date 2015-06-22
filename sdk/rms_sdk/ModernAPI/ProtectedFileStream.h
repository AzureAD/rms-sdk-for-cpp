/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_PROTECTEDFILESTREAM_H_
#define _RMS_LIB_PROTECTEDFILESTREAM_H_

#include <CryptoAPI.h>
#include "UserPolicy.h"
#include "ModernAPIExport.h"
#include "CacheControl.h"

namespace rmscore {
namespace pfile {
class PfileHeader;
}

namespace modernapi {
class ProtectedFileStream;

struct DLL_PUBLIC_RMS GetProtectedFileStreamResult
{
  GetUserPolicyResultStatus           m_status;
  std::shared_ptr<std::string>        m_referrer;
  std::shared_ptr<ProtectedFileStream>m_stream;

  GetProtectedFileStreamResult(GetUserPolicyResultStatus           status,
                               std::shared_ptr<std::string>        referrer,
                               std::shared_ptr<ProtectedFileStream>stream);
};

class DLL_PUBLIC_RMS ProtectedFileStream : public rmscrypto::api::IStream {
public:

  virtual ~ProtectedFileStream();
  virtual std::shared_future<int64_t>ReadAsync(uint8_t    *pbBuffer,
                                               int64_t     cbBuffer,
                                               int64_t     cbOffset,
                                               std::launch launchType)
  override;
  virtual std::future<bool>FlushAsync(
    std::launch launchType)
  override;

  virtual int64_t Read(uint8_t *pbBuffer,
                       int64_t  cbBuffer)
  override;
  virtual int64_t Write(
    const uint8_t *cpbBuffer,
    int64_t        cbBuffer)
  override;
  virtual bool                                        Flush() override;

  virtual rmscrypto::api::SharedStream                Clone() override;

  virtual void                                        Seek(uint64_t u64Position)
  override;
  virtual bool                                        CanRead()  const  override;
  virtual bool                                        CanWrite() const  override;
  virtual uint64_t                                    Position()        override;
  virtual uint64_t                                    Size()            override;
  virtual void                                        Size(uint64_t u64Value)
  override;

  static std::shared_ptr<GetProtectedFileStreamResult>Acquire(
    rmscrypto::api::SharedStream stream,
    const std::string          & userId,
    IAuthenticationCallback    & authenticationCallback,
    IConsentCallback           & consentCallback,
    PolicyAcquisitionOptions     options,
    ResponseCacheFlags           cacheMask
      = static_cast<ResponseCacheFlags>(RESPONSE_CACHE_INMEMORY |
                                        RESPONSE_CACHE_ONDISK |
                                        RESPONSE_CACHE_CRYPTED));

  static std::shared_ptr<ProtectedFileStream>Create(
    std::shared_ptr<UserPolicy>  policy,
    rmscrypto::api::SharedStream stream,
    const std::string          & originalFileExtension);

  std::shared_ptr<UserPolicy>Policy() {
    return m_policy;
  }

  std::string OriginalFileExtension() {
    return m_originalFileExtension;
  }

private:

  // for inner use only
  ProtectedFileStream(rmscrypto::api::SharedStream pImpl,
                      std::shared_ptr<UserPolicy>  policy,
                      const std::string          & originalFileExtension);


  virtual std::shared_future<int64_t>WriteAsync(const uint8_t *cpbBuffer,
                                                int64_t        cbBuffer,
                                                int64_t        cbOffset,
                                                std::launch    launchType)
  override;

  static ProtectedFileStream* CreateProtectedFileStream(
    std::shared_ptr<UserPolicy>        policy,
    rmscrypto::api::SharedStream       stream,
    std::shared_ptr<pfile::PfileHeader>header);

private:

  std::shared_ptr<UserPolicy> m_policy;
  std::string m_originalFileExtension;
  std::shared_ptr<IStream> m_pImpl;
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_PROTECTEDFILESTREAM_H_
