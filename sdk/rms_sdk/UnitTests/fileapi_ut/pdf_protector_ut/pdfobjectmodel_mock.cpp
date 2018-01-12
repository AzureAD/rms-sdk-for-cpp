/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "pdfobjectmodel_mock.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include "BlockBasedProtectedStream.h"
#include "RMSExceptions.h"
#include "Common/CommonTypes.h"

namespace rmscore_unittest {
namespace fileapi_unittest {
class PDFEncryptStream_mock;
class PDFCryptoHandler_mock;
PDFCryptoHandler_mock::PDFCryptoHandler_mock() {

}

PDFCryptoHandler_mock::~PDFCryptoHandler_mock() {
}

uint32_t PDFCryptoHandler_mock::DecryptGetSize(uint32_t src_size) {
  return src_size;
}

void PDFCryptoHandler_mock::DecryptStart(uint32_t objnum, uint32_t gennum) {
  FILEAPI_UNREFERENCED_PARAMETER(gennum);
  FILEAPI_UNREFERENCED_PARAMETER(objnum);
}

bool PDFCryptoHandler_mock::DecryptStream(char* src_buf, uint32_t src_size, pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  char* buff_dest = new char[src_size+4];
  for(unsigned int i = 0; i < src_size; i++)
    buff_dest[i] = src_buf[i] - 1;
  dest_buf->AppendBlock(buff_dest, src_size);
  delete [] buff_dest;
  buff_dest = nullptr;
  return true;
}

bool PDFCryptoHandler_mock::DecryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  FILEAPI_UNREFERENCED_PARAMETER(dest_buf);
  return true;
}

uint32_t PDFCryptoHandler_mock::EncryptGetSize(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size) {
  FILEAPI_UNREFERENCED_PARAMETER(src_buf);
  FILEAPI_UNREFERENCED_PARAMETER(version);
  FILEAPI_UNREFERENCED_PARAMETER(objnum);
  return src_size;
}

bool PDFCryptoHandler_mock::EncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, char* dest_buf, uint32_t* dest_size) {
  FILEAPI_UNREFERENCED_PARAMETER(version);
  FILEAPI_UNREFERENCED_PARAMETER(objnum);
  FILEAPI_UNREFERENCED_PARAMETER(dest_size);
  for(unsigned int i = 0; i < src_size; i++)
    dest_buf[i] = src_buf[i] + 1;
  return true;
}

bool PDFCryptoHandler_mock::ProgressiveEncryptStart(uint32_t objnum, uint32_t version, uint32_t raw_size) {
  FILEAPI_UNREFERENCED_PARAMETER(version);
  FILEAPI_UNREFERENCED_PARAMETER(objnum);
  FILEAPI_UNREFERENCED_PARAMETER(raw_size);
  return false;
}

bool PDFCryptoHandler_mock::ProgressiveEncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  FILEAPI_UNREFERENCED_PARAMETER(version);
  FILEAPI_UNREFERENCED_PARAMETER(objnum);
  FILEAPI_UNREFERENCED_PARAMETER(dest_buf);
  FILEAPI_UNREFERENCED_PARAMETER(src_buf);
  FILEAPI_UNREFERENCED_PARAMETER(src_size);
  return true;
}

bool PDFCryptoHandler_mock::ProgressiveEncryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  FILEAPI_UNREFERENCED_PARAMETER(dest_buf);
  return true;
}

PDFSecurityHandler_mock::PDFSecurityHandler_mock(){
  crypto_handler_ = nullptr;
}

PDFSecurityHandler_mock::~PDFSecurityHandler_mock() {
  crypto_handler_.reset();
  crypto_handler_ = nullptr;
}

bool PDFSecurityHandler_mock::OnInit(unsigned char *publishing_license, uint32_t publishing_license_size) {
  std::vector<uint8_t> vec_publishing_license(publishing_license_size);
  memcpy(reinterpret_cast<uint8_t *>(&vec_publishing_license[0]), publishing_license, publishing_license_size);
  return true;
}

std::shared_ptr<pdfobjectmodel::PDFCryptoHandler> PDFSecurityHandler_mock::CreateCryptoHandler() {
  crypto_handler_ = std::make_shared<PDFCryptoHandler_mock>();
  return crypto_handler_;
}

PDFSharedStream_mock::PDFSharedStream_mock(std::shared_ptr<std::iostream> stream){
  m_iBackingStream = stream;
}

PDFSharedStream_mock::~PDFSharedStream_mock() {
}

void PDFSharedStream_mock::Release() {
  return;
}

uint64_t PDFSharedStream_mock::GetSize() {
  m_iBackingStream->seekg(0, std::ios::end);
  uint64_t size = m_iBackingStream->tellg();
  return size;
}

bool PDFSharedStream_mock::IsEOF() {
  return false;
}

uint64_t PDFSharedStream_mock::GetPosition() {
  return 2;
}

bool PDFSharedStream_mock::ReadBlock(void* buffer, uint64_t offset, uint64_t size) {
  m_iBackingStream->seekg(offset);
  m_iBackingStream->read(reinterpret_cast<char*>(buffer), size);
  return true;
}

uint64_t PDFSharedStream_mock::ReadBlock(void* buffer, uint64_t size) {
  int64_t read = m_iBackingStream->readsome(reinterpret_cast<char*>(buffer), size);
  return read;
}

bool PDFSharedStream_mock::WriteBlock(const void* buffer, uint64_t offset, uint64_t size) {
  m_iBackingStream->seekp(offset);
  m_iBackingStream->write(reinterpret_cast<const char*>(buffer), size);
  return true;
}


bool PDFSharedStream_mock::Flush() {
  m_iBackingStream->flush();
  return true;
}

} // namespace fileapi_unittest
} // namespace rmscore_unittest
