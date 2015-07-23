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

  /*!
  @brief Returns an asynchronous byte reader object.
  @param pbBuffer The buffer into which the asynchronous read operation places the bytes that are read. 
  @param cbBuffer The number of bytes to read that is less than or equal to the capacity of the buffer.
  @param cbOffset BRP072315 - ???
  @param launchType BRP072315 - ???
  @return Count of bytes read.
  */
  virtual std::shared_future<int64_t> ReadAsync(uint8_t    *pbBuffer,
                                                int64_t     cbBuffer,
                                                int64_t     cbOffset,
                                                std::launch launchType)
  override;

  /*!
  @brief Writes data asynchronously to the protected file.
  @param pbBuffer The buffer into which the asynchronous write operation writes. 
  @param cbBuffer BRP072315 - ???
  @param cbOffset BRP072315 - ???
  @param launchType BRP072315 - ???
  @return Count of bytes read.
  */
  virtual std::shared_future<int64_t> WriteAsync(const uint8_t *cpbBuffer,
                                                int64_t        cbBuffer,
                                                int64_t        cbOffset,
                                                std::launch    launchType)
  override;

  /*!
  @brief Flushes the data asynchronously to the protected file.
  @param launchType BRP072315 - ???
  @return The stream flush operation. 
          Null is returned if an exception occurs. 
  */
  virtual std::future<bool> FlushAsync(std::launch launchType) override;

  /*!
  @brief Returns an byte reader object. 
  @param pbBuffer The buffer into which the read operation places the bytes that are read. 
  @param cbBuffer The number of bytes to read that is less than or equal to the capacity of the buffer.
  @return Count of bytes read.
  */
  virtual int64_t Read(uint8_t *pbBuffer,
                       int64_t  cbBuffer)
  override;

  /*!
  @brief Writes data to the protected file.
  @param pbBuffer The buffer into which the write operation writes. 
  @param cbBuffer BRP072315 - ???
  @return Count of bytes written.
  */
  virtual int64_t Write(const uint8_t *cpbBuffer,
                        int64_t        cbBuffer)
  override;

  /*!
  @brief Flushes the data to the protected file.
  @return For the stream flush operation; null is returned if an exception occurs. 
          Upon completion, IAsyncOperation.GetResults returns True if the file has 
		  been successfully flushed; otherwise, false. 
  */
  virtual bool Flush() override;

  /*!
  @brief Creates a new instance of ProtectedFileStream over the protected file.
  @return A ProtectedFileStream object that represents the new stream. 
          The initial, internal position of the stream is 0. 
		  The internal position and lifetime of the new stream are independent 
		  from the position and lifetime of the cloned stream. 
  */
  virtual rmscrypto::api::SharedStream Clone() override;

  /*!
  @brief Sets the current position to the specified number of bytes from the start of the file.
  @param u64Position Number of bytes from the start of the file
  */
  virtual void Seek(uint64_t u64Position) override;

  /*!
  @brief Gets a value that indicates whether the file can be read from.
  @return True if the file can be read from; otherwise false.
  */
  virtual bool CanRead() const override;

  /*!
  @brief Gets a value that indicates whether the file can be written to.
  @return True if the file can be written to; otherwise false.
  */
  virtual bool CanWrite() const override;

  /*!
  @brief Gets the current file position in bytes.
  @return Current file position in bytes.
  */
  virtual uint64_t Position() override;

  /*!
  @brief Gets the size of the protected data in bytes.
  @return File stream.
  */
  virtual uint64_t Size() override;

  /*!
  @brief Sets the size of the protected data in bytes.
  @param u64Value Number of bytes of protected data.
  */
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
  static std::shared_ptr<GetProtectedFileStreamResult> Acquire(
    rmscrypto::api::SharedStream stream,
    const std::string          & userId,
    IAuthenticationCallback    & authenticationCallback,
    IConsentCallback           & consentCallback,
    PolicyAcquisitionOptions     options,
    ResponseCacheFlags           cacheMask
      = static_cast<ResponseCacheFlags>(RESPONSE_CACHE_INMEMORY |
                                        RESPONSE_CACHE_ONDISK |
                                        RESPONSE_CACHE_CRYPTED));

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
