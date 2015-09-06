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
} // namespace pfile

namespace modernapi {
class ProtectedFileStream;

/*!
  @brief The result of the ProtectedFileStream::Acquire operation
*/

struct DLL_PUBLIC_RMS GetProtectedFileStreamResult
{
  GetUserPolicyResultStatus            m_status;
  std::shared_ptr<std::string>         m_referrer;
  std::shared_ptr<ProtectedFileStream> m_stream;

  GetProtectedFileStreamResult(GetUserPolicyResultStatus            status,
                               std::shared_ptr<std::string>         referrer,
                               std::shared_ptr<ProtectedFileStream> stream);
};

/*!
  @brief Wraps a std::iostream to provide transparent encryption and decryption
  on read and write.

  Use ProtectedFileStream::Acquire when working with encrypted (PFile) content.
  Use ProtectedFileStream::Create to wrap and encrypt a new stream.
*/
class DLL_PUBLIC_RMS ProtectedFileStream : public rmscrypto::api::IStream {
public:

  virtual ~ProtectedFileStream();
  virtual std::shared_future<int64_t> ReadAsync(uint8_t    *pbBuffer,
                                                int64_t     cbBuffer,
                                                int64_t     cbOffset,
                                                std::launch launchType)
  override;

  virtual std::shared_future<int64_t> WriteAsync(const uint8_t *cpbBuffer,
                                                int64_t        cbBuffer,
                                                int64_t        cbOffset,
                                                std::launch    launchType)
  override;

  virtual std::future<bool> FlushAsync(std::launch launchType) override;

  virtual int64_t Read(uint8_t *pbBuffer,
                       int64_t  cbBuffer)
  override;

  virtual int64_t Write(const uint8_t *cpbBuffer,
                        int64_t        cbBuffer)
  override;

  virtual bool Flush() override;
  virtual rmscrypto::api::SharedStream Clone() override;
  virtual void Seek(uint64_t u64Position) override;
  virtual bool CanRead() const override;
  virtual bool CanWrite() const override;
  virtual uint64_t Position() override;
  virtual uint64_t Size() override;
  virtual void Size(uint64_t u64Value) override;

  /*!
   @brief Wrap an existing stream as a protected stream.

   Creates a ProtectedFileStream object from a backing stream. This method is used to open PFILEs. The PFILE is loaded from the specified
   backing stream. After ProtectedFileStream object is created, apps can use ProtectedFileStream::Policy property to get the UserPolicy object
   that defines the policy used to protect the PFILE. The resulting ProtectedFileStream can be used to read the contents of the PFILE
   (as a regular IRandomAccessStream).

   @param stream The std::iostream to be wrapped.
   @param userId The email address of the user consuming the protected content.
                 This parameter is also used as a hint for auth dialogs.
   @param authenticationCallback A callback which will return authentication details for the current user.
   @param consentCallback A consent callback to ensure user consent.
   @param options
   @param cacheMask How API responses should be cached.
   @return A GetProtectedFileStreamResult struct with status and pointer to wrapped stream.
  */
  static std::shared_ptr<GetProtectedFileStreamResult> Acquire(rmscrypto::api::SharedStream stream,
    const std::string          & userId,
    IAuthenticationCallback    & authenticationCallback,
    IConsentCallback           * consentCallback,
    PolicyAcquisitionOptions     options,
    ResponseCacheFlags           cacheMask
      = static_cast<ResponseCacheFlags>(RESPONSE_CACHE_INMEMORY |
                                        RESPONSE_CACHE_ONDISK |
                                        RESPONSE_CACHE_CRYPTED),
    std::shared_ptr<std::atomic<bool> > cancelState = nullptr);

  /*!
  @brief Wrap a new stream as a protected stream.

  Creates a new ProtectedFileStream from a UserPolicy object and a backing stream. This method is used to create new PFiles.
  The PFile is written to the backing stream. The policy used to protect the PFile is defined by the specified
  UserPolicy object. The resulting ProtectedFileStream can be used to write the actual content of the PFile using APIs from std::io.

  @param policy The UserPolicy object that defines the policy used to protect the created PFile
  @param stream The backing stream, where encrypted content will be written.
  @param originalFileExtension The file extension of the original unprotected file.
  @return A ProtectedFileStream.
  */
  static std::shared_ptr<ProtectedFileStream> Create(
    std::shared_ptr<UserPolicy>  policy,
    rmscrypto::api::SharedStream stream,
    const std::string          & originalFileExtension);

  std::shared_ptr<UserPolicy> Policy() { return m_policy; }

  std::string OriginalFileExtension() { return m_originalFileExtension; }

private:

  ProtectedFileStream(rmscrypto::api::SharedStream pImpl,
                      std::shared_ptr<UserPolicy>  policy,
                      const std::string          & originalFileExtension);


  static ProtectedFileStream* CreateProtectedFileStream(
    std::shared_ptr<UserPolicy>         policy,
    rmscrypto::api::SharedStream        stream,
    std::shared_ptr<pfile::PfileHeader> header);

private:

  std::shared_ptr<UserPolicy> m_policy;
  std::string m_originalFileExtension;
  std::shared_ptr<IStream> m_pImpl;

}; // class ProtectedFileStream
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_PROTECTEDFILESTREAM_H_
