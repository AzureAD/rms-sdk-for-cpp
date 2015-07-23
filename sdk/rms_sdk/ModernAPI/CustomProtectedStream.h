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
 *        Important - RMS applications that use CustomProtectedStream may be 
 *        incompatible with SharePoint, Exchange, and other RMS applications. 
 *        For most applications, it is recommended that you use ProtectedFileStream instead.
 */
class DLL_PUBLIC_RMS CustomProtectedStream : public rmscrypto::api::IStream {
public:

  /*!
   * @brief Creates a CustomProtectedStream object based on the specified protection policy 
   *        and a backing stream. 
   *
   *        The CustomProtectedStream can be used for both encrypting and decrypting content. 
   *        If the backing stream already contains encrypted content, you can use the stream 
   *        reading functions to decrypt the content. You can also use stream writing functions 
   *        to encrypt the content and write the encrypted content to the backing stream. 
   *
   *        Warning - To avoid data loss and/or corruption, FlushAsync must be called if you modify 
   *        the created stream or before object disposal.
   *
   *        Remarks - You specify a range (contentStartPosition, contentSize) where the encrypted content 
   *                  is located in the backing stream.
   *                  If there is no existing content in the backing stream (for example, if this is a new file), 
   *                    specify contentSize as zero. 
   *                  If the existing content ends at the end of the stream, you can specify ulong.MaxValue for contentSize.
   *                  The contentSize parameter is needed only for the cases when there is non-encrypted app-specific 
   *                    content after the encrypted content. This is because the framework needs to know where the encrypted 
   *                    content ends when performing decryption.
   *                  The contentSize parameter is specified in terms of the encrypted content for example, it does include 
   *                    the size of the CBC padding. 
   *                  If the range defined by the parameters contentStartPosition and contentSize is not empty 
   *                    (for example, contentSize != 0), it must address an entire segment of encrypted content; 
   *                    that is, it must start from block 0 and must have a final block in the CBC case or should 
   *                    be 16-byte aligned in the ECB case.
   *
   *        Warning- To avoid data loss and/or corruption, FlushAsync must be called if you modify the created stream or before object disposal.
   * 
   *        Warning - This method must be called on a UI thread. Calling it on a worker thread may result in an unexpected behavior.
   *
   * @param policy The user policy to apply to protect the content.
   * @param stream The backing stream.
   * @param contentStartPosition The start position of the stream.
   * @param contentSize The size of the encrypted content.
   * @return A CustomProtectedStream object that can be used to write content to the file. 
   */
  static std::shared_ptr<CustomProtectedStream>Create(
    std::shared_ptr<UserPolicy>  policy,
    rmscrypto::api::SharedStream stream,
    uint64_t                     contentStartPosition,
    uint64_t                     contentSize);
  /*!
  * @brief Calculates the length of the encrypted content from the length of the clear content.
  * @param policy Pointer to userPolicy object.
  * @param contentLenght Length of the clear content.
  * @return The encrypted content length.
  */
  static uint64_t GetEncryptedContentLength(
    std::shared_ptr<UserPolicy>policy,
    uint64_t                   contentLength);

  /*!
  * @brief Returns an asynchronous byte reader object.
  * @param pbBuffer The buffer into which the asynchronous read operation places the bytes that are read. 
  * @param cbBuffer 
  * @param cbOffset
  * @param launchType
  * @return Count of bytes read.
  */
  virtual std::shared_future<int64_t>ReadAsync(uint8_t    *pbBuffer,
                                               int64_t     cbBuffer,
                                               int64_t     cbOffset,
                                               std::launch launchType)
  override;

  /*!
  * @brief Writes data asynchronously to the backing file.
  * @param cpbBuffer Pointer to the buffer into which the asynchronous write operation writes. 
  * @param cbBuffer
  * @param cbOffset
  * @param launchType
  * @return Count of bytes written.
  */
  virtual std::shared_future<int64_t>WriteAsync(const uint8_t *cpbBuffer,
                                                int64_t        cbBuffer,
                                                int64_t        cbOffset,
                                                std::launch    launchType)
  override;

  /*!
  * @brief Flushes the data asynchronously to the backing file.
  * @param launchType
  * @return The stream flush operation. null is returned if an exception occurs. 
  *         Upon completion, IAsyncOperation.GetResults returns True if the file 
  *         has been successfully flushed; otherwise, false. 
  */
  virtual std::future<bool>            FlushAsync(std::launch launchType)
  override;

  /*!
  * @brief Reads data from the backing file.
  * @param pbBuffer The buffer into which the read operation places the bytes that are read. 
  * @param cbBuffer The number of bytes to read that is less than or equal to the capacity of the buffer.
  * @return Bytes read.
  */
  virtual int64_t                      Read(uint8_t *pbBuffer,
                                            int64_t  cbBuffer) override;

  /*!
  * @brief Writes data to the backing file.
  * @param cpbBuffer The buffer into which the write operation writes. 
  * @param cbBuffer The size of the buffer.
  * @return Bytes written.
  */
  virtual int64_t                      Write(const uint8_t *cpbBuffer,
                                             int64_t        cbBuffer) override;

  /*!
  * @brief Flushes the data to the backing file.
  * @return The stream flush operation. null is returned if an exception occurs. 
  *         Upon completion, IAsyncOperation.GetResults returns True if the file 
  *         has been successfully flushed; otherwise, false. 
  */
  virtual bool                         Flush() override;

  /*!
  * @brief Creates a new instance of CustomProtectedStream over the protected content.
  * @return A CustomProtectedStream object that represents the new stream. The initial, 
  *         internal position of the stream is 0 (zero). The internal position and lifetime 
  *         of the new stream are independent from the position and lifetime of the cloned stream.
  */
  virtual rmscrypto::api::SharedStream Clone() override;

  /*!
  * @brief Sets the current position to the specified number of bytes from the start of the file.
  * @param u64Position The number of bytes from the start of the file at which to set the position. 
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
  * @brief Gets the current file position in bytes.
  * @return The current file position in bytes.
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

  //
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
