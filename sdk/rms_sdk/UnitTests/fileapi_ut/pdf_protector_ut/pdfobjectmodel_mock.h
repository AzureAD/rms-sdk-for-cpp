/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_PDFPROTECTOR_CHILD_H_
#define RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_PDFPROTECTOR_CHILD_H_

#include "Protector.h"
#include "BlockBasedProtectedStream.h"
#include "FileAPIStructures.h"
#include "PDFObjectModel/pdf_object_model.h"
//#include "CryptoAPI.h"
#include "Gtest/gtest.h"
#include "gmock/gmock.h"
namespace rmscore_unittest{
namespace fileapi_unittest {

namespace pdfobjectmodel = rmscore::pdfobjectmodel;
using namespace rmscore::pdfobjectmodel;
#define PROGRESSIVE_ENCRYPT_TEMP_FILE ".RMS.PE.temp"
#define CREATOR_STREAM_TEMP_FILE ".creatorstream.temp"
class PDFCryptoHandler_mock;
class PDFSharedStream_mock;
class PDFSecurityHandler_mock;

class PDFSharedStream_mock : public pdfobjectmodel::PDFDataStream {
 public:
  explicit PDFSharedStream_mock(std::shared_ptr<std::iostream> stream);

  virtual ~PDFSharedStream_mock();

  virtual void Release();

  virtual uint64_t GetSize();

  virtual bool IsEOF();

  virtual uint64_t GetPosition();

  virtual bool ReadBlock(void* buffer, uint64_t offset, uint64_t size);

  virtual uint64_t ReadBlock(void* buffer, uint64_t size);

  virtual bool WriteBlock(const void* buffer, uint64_t offset, uint64_t size);

  virtual bool Flush();

 private:

  std::shared_ptr<std::iostream>m_iBackingStream;
};

class PDFCryptoHandler_mock : public rmscore::pdfobjectmodel::PDFCryptoHandler {
 public:
  explicit PDFCryptoHandler_mock();

  virtual ~PDFCryptoHandler_mock();

  virtual uint32_t DecryptGetSize(uint32_t src_size);

  virtual void DecryptStart(uint32_t objnum, uint32_t gennum);

  virtual bool DecryptStream(char* src_buf, uint32_t src_size, pdfobjectmodel::PDFBinaryBuf* dest_buf);

  virtual bool DecryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf);

  virtual uint32_t EncryptGetSize(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size);

  virtual bool EncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, char* dest_buf, uint32_t* dest_size);

  virtual bool ProgressiveEncryptStart(uint32_t objnum, uint32_t version, uint32_t raw_size);

  virtual bool ProgressiveEncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, pdfobjectmodel::PDFBinaryBuf* dest_buf);

  virtual bool ProgressiveEncryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf);
};


class PDFSecurityHandler_mock : public rmscore::pdfobjectmodel::PDFSecurityHandler {
 public:
  PDFSecurityHandler_mock();

  virtual ~PDFSecurityHandler_mock();

  virtual bool OnInit(unsigned char* publishing_license, uint32_t publishing_license_size);

  virtual std::shared_ptr<pdfobjectmodel::PDFCryptoHandler> CreateCryptoHandler();

private:
  std::shared_ptr<pdfobjectmodel::PDFCryptoHandler> crypto_handler_;
};
} // namespace fileapi_unittest
} // namespace rmscore_unittest

#endif // RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_PDFPROTECTOR_CHILD_H_

