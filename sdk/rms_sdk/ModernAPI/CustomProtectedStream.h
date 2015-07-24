/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_CUSTOMPROTECTEDSTREAM_H_
#define _RMS_LIB_CUSTOMPROTECTEDSTREAM_H_

#include "UserPolicy.h"
#include "IStream.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {

/*!
 * @brief Used to access files that use a custom protection format.
 *
 *        **Important** - RMS applications that use CustomProtectedStream may be
 *        incompatible with SharePoint, Exchange, and other RMS applications.
 *        For most applications, it is recommended that you use
 *        ProtectedFileStream instead.
 */
class DLL_PUBLIC_RMS CustomProtectedStream : public rmscrypto::api::IStream {
public:

  /*!
   * @brief Creates a CustomProtectedStream object based on the specified
   *        protection policy and a backing stream.
   *
   *        The CustomProtectedStream can be used for both encrypting and
   *        decrypting content.
   *        If the backing stream already contains encrypted content,
   *        you can use the stream Read* methods to decrypt and read it.
   *        You can also use stream Write* methods to encrypt and write
   *        content to the backing stream.
   *
   *        **Warning** - To avoid data loss and/or corruption, the Flush*
   *        methods must be called if you modify the created stream before the
   *        stream goes out of scope or is manually disposed.
   *
   *        **Warning** - This method must be called on a UI thread. Calling it
   *        on a worker thread may result in an unexpected behavior.
   *
   * @remarks You must specify a range (contentStartPosition, contentSize)
   *          where the encrypted content is located in the backing stream.
   *          If there is no existing content in the backing stream
   *          (e.g. a new file), specify contentSize as 0.
   *          If the existing content ends at the end of the stream, you can
   *          specify ulong.MaxValue for contentSize.
   *          The contentSize parameter is needed only for those cases
   *          where there is non-encrypted app-specific content *after* the
   *          encrypted content; in this case the method needs to know where the
   *          encrypted content ends when performing decryption.
   *          The contentSize parameter is specified in terms of the encrypted
   *          content. For example, it should include the size of the CBC
   *          padding.
   *          If the range defined by the parameters contentStartPosition and
   *          contentSize is not empty (i.e. contentSize != 0), it must address
   *          an entire segment of encrypted content; that is, it must start
   *          from block 0 and must have a final block (for CBC) or be 16-byte
   *          aligned (for ECB).
   *
   * @param policy The user policy to apply to protect the content.
   * @param stream The backing stream.
   * @param contentStartPosition The start position of encrypted content
                                 within the stream.
   * @param contentSize The size of the encrypted content within the stream.
   * @return A CustomProtectedStream object that can be used to write content to
             the encrypted portion of the backing stream.
   */
  static std::shared_ptr<CustomProtectedStream> Create(
    std::shared_ptr<UserPolicy>  policy,
    rmscrypto::api::SharedStream stream,
    uint64_t                     contentStartPosition,
    uint64_t                     contentSize);

  /*!
  * @brief Calculates the length of the encrypted content from the length of the
           plaintext content.
  * @param policy Pointer to userPolicy object.
  * @param contentLenght Length of the plaintext content.
  * @return The encrypted content length.
  */
  static uint64_t GetEncryptedContentLength(
    std::shared_ptr<UserPolicy>policy,
    uint64_t                   contentLength);

  /*!
  * @brief Requests contents of the stream to be read asynchronously into
           pbBuffer, and returns a std::shared_future to track status of the
           asynchronous operation. Call .get() or .wait() on this shared_future
           to synchronously await its completion.
  * @param pbBuffer The buffer into which the (asynchronous) read operation
                    should place read bytes.
  * @param cbBuffer Count of bytes in this buffer.
  * @param cbOffset Offset into the buffer where bytes should begin being
                    written.
  * @param launchType Can be std::launch::async and/or std::launch::deferred.
                      If deferred, read will take place "lazily" - i.e. only
                      when get() or wait() is called.
  * @return (Future of) count of bytes read.
  */
  virtual std::shared_future<int64_t>ReadAsync(uint8_t    *pbBuffer,
                                               int64_t     cbBuffer,
                                               int64_t     cbOffset,
                                               std::launch launchType)
  override;

  /*!
  * @brief Requests that data from *cpbBuffer be written asynchronously to
           stream, and returns a std::shared_future to track status of the
           asynchronous operation. Call .get() or .wait() on this shared_future
           to synchronously await its completion.
  * @param cpbBuffer Pointer to the buffer from which the write operation will
                     read contents to be written.
  * @param cbBuffer  Count of bytes in cpbBuffer.
  * @param cbOffset  Offset into cpbBuffer where operation will begin reading
                     from for write.
  * @param launchType Can be std::launch::async and/or std::launch::deferred.
                      If deferred, write will take place "lazily" - i.e. only
                      when get() or wait() is called.
  * @return (Future of) count of bytes written.
  */
  virtual std::shared_future<int64_t>WriteAsync(const uint8_t *cpbBuffer,
                                                int64_t        cbBuffer,
                                                int64_t        cbOffset,
                                                std::launch    launchType)
  override;

  /*!
  * @brief Requests that pending data be flushed asynchronously to
           stream, and returns a std::shared_future to track status of the
           asynchronous operation. Call .get() or .wait() on this shared_future
           to synchronously await its completion.
  * @param launchType Can be std::launch::async and/or std::launch::deferred.
                      If deferred, flush will take place "lazily" - i.e. only
                      when get() or wait() is called.
  * @return (Future of) the result of the flush operation. True if data was
            successfully flushed; otherwise False.
  */
  virtual std::future<bool>            FlushAsync(std::launch launchType)
  override;

  /*!
  * @brief Reads data from the backing stream.
  * @param pbBuffer The buffer into which the read operation puts the bytes that
                    are read.
  * @param cbBuffer The number of bytes available in the buffer.
  * @return Number of bytes actually read.
  */
  virtual int64_t                      Read(uint8_t *pbBuffer,
                                            int64_t  cbBuffer) override;

  /*!
  * @brief Writes data to the backing stream.
  * @param cpbBuffer The buffer from which the write operation reads data
                     to be written.
  * @param cbBuffer The size of the buffer.
  * @return Number of bytes written.
  */
  virtual int64_t                      Write(const uint8_t *cpbBuffer,
                                             int64_t        cbBuffer) override;

  /*!
  * @brief Flushes the data to the backing stream.
  * @return The result of the flush operation. True if data was
            successfully flushed; otherwise False.
  */
  virtual bool                         Flush() override;

  /*!
  * @brief Creates a copy of the CustomProtectedStream.
  * @return A CustomProtectedStream object that represents the new stream.
           The initial seek position of the stream is 0. The seek position and lifetime of the new stream are independent of the position and lifetime of the original stream.
  */
  virtual rmscrypto::api::SharedStream Clone() override;

  /*!
  * @brief Sets the current position to the specified offset in the stream.
  * @param u64Position The offset in the stream to move to.
  */
  virtual void                         Seek(uint64_t u64Position) override;

  /*!
  * @brief Gets a value that indicates whether the file can be read from.
  * @return True if the file can be read from; otherwise false.
  */
  virtual bool                         CanRead()  const           override;

  /*!
  * @brief Gets a value that indicates whether the file can be written to.
  * @return True if the file can be written to; otherwise false.
  */
  virtual bool                         CanWrite() const           override;

  /*!
  * @brief Gets the current file seek offset.
  * @return The current file seek offset.
  */
  virtual uint64_t                     Position()                 override;

  /*!
  * @brief Gets the size of the protected data in bytes.
  * @return Size of the protected data in bytes.
  */
  virtual uint64_t                     Size()                     override;

  /*!
  * @brief Sets the size of the protected data in bytes.
  * @param u64Value Size of the protected data in bytes.
  */
  virtual void                         Size(uint64_t u64Value)    override;

  virtual ~CustomProtectedStream();

protected:

  CustomProtectedStream(std::shared_ptr<IStream>pImpl);
  std::shared_ptr<IStream>GetImpl();

private:

  std::shared_ptr<IStream> m_pImpl;
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_CUSTOMPROTECTEDSTREAM_H_
