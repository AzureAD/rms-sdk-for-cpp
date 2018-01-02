/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/
#include "PDFProtector.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

#include "BlockBasedProtectedStream.h"
#include "ModernAPI/RMSExceptions.h"
#include "Common/CommonTypes.h"
#include "Core/ProtectionPolicy.h"
#include "Platform/Logger/Logger.h"

namespace logger = rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

//////////////////////////////////////////////////////////////////////////
// class PDFDataStreamImpl
PDFDataStreamImpl::PDFDataStreamImpl(rmscrypto::api::SharedStream ioStream)
    : shared_io_stream_(ioStream) {

}

PDFDataStreamImpl::~PDFDataStreamImpl() {
}

void PDFDataStreamImpl::Release() {
}

uint64_t PDFDataStreamImpl::GetSize() {
  uint64_t size = shared_io_stream_->Size();
  return size;
}

bool PDFDataStreamImpl::IsEOF() {
  uint64_t size = shared_io_stream_->Size();
  uint64_t pos = shared_io_stream_->Position();
  if (pos == size - 1) {
    return true;
  }
  return false;
}

uint64_t PDFDataStreamImpl::GetPosition() {
  uint64_t pos = shared_io_stream_->Position();
  return pos;
}

bool PDFDataStreamImpl::ReadBlock(void* buffer, uint64_t offset, uint64_t size) {
  shared_io_stream_->Seek(offset);
  shared_io_stream_->Read(reinterpret_cast<unsigned char*>(buffer), size);
  return true;
}

uint64_t PDFDataStreamImpl::ReadBlock(void* buffer, uint64_t size) {
  int64_t read = shared_io_stream_->Read(reinterpret_cast<unsigned char*>(buffer), size);
  return read;
}

bool PDFDataStreamImpl::WriteBlock(const void* buffer, uint64_t offset, uint64_t size) {
  shared_io_stream_->Seek(offset);
  shared_io_stream_->Write(reinterpret_cast<const unsigned char*>(buffer), size);
  return true;
}

bool PDFDataStreamImpl::Flush() {
  bool flush_result = shared_io_stream_->Flush();
  return flush_result;
}

////////////////////////////////////////////////////////////////////////////
/// class PDFProtector
PDFCryptoHandlerImpl::PDFCryptoHandlerImpl(std::shared_ptr<PDFProtector> pdf_protector) {
  pdf_protector_ = pdf_protector;
  progressive_start_ = false;

  data_to_be_decrypted_ = nullptr;
  obj_num_ = 0;

  shared_protected_stream_ = nullptr;
  output_IOS_ = nullptr;
  output_shared_stream_ = nullptr;
}

PDFCryptoHandlerImpl::~PDFCryptoHandlerImpl() {
  pdf_protector_ = nullptr;
}

uint32_t PDFCryptoHandlerImpl::DecryptGetSize(uint32_t src_size) {
  return src_size;
}

void PDFCryptoHandlerImpl::DecryptStart(uint32_t objnum, uint32_t gennum) {
  FILEAPI_UNREFERENCED_PARAMETER(gennum);
  obj_num_ = objnum;
  data_to_be_decrypted_ = std::make_shared<std::stringstream>();
}

bool PDFCryptoHandlerImpl::DecryptStream(
    char* src_buf,
    uint32_t src_size,
    pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  FILEAPI_UNREFERENCED_PARAMETER(dest_buf);
  data_to_be_decrypted_->write(src_buf, src_size);
  return true;
}

bool PDFCryptoHandlerImpl::DecryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  data_to_be_decrypted_->seekg(0, std::ios::end);
  uint64_t count = data_to_be_decrypted_->tellg();
  if (count <= 0) {
    obj_num_ = 0;
    data_to_be_decrypted_ = nullptr;
    return true;
  }

  std::shared_ptr<std::iostream> protectedIOS = data_to_be_decrypted_;
  auto input_shared_stream = rmscrypto::api::CreateStreamFromStdStream(protectedIOS);
  auto protected_stream = pdf_protector_->CreateProtectedStream(input_shared_stream, count);

  std::shared_ptr<std::stringstream> outputSS = std::make_shared<std::stringstream>();
  std::shared_ptr<std::iostream> outputIOS = outputSS;
  auto output_shared_stream = rmscrypto::api::CreateStreamFromStdStream(protectedIOS);

  pdf_protector_->DecryptStream(output_shared_stream, protected_stream, count);

  output_shared_stream->Seek(std::ios::beg);
  auto size = output_shared_stream->Size();
  std::vector<uint8_t> decrypted_data = output_shared_stream->Read(size);

  char buf_dest_size[4];
  memset(buf_dest_size, 0, 4 * sizeof(char));
  buf_dest_size[0] = decrypted_data[3];
  buf_dest_size[1] = decrypted_data[2];
  buf_dest_size[2] = decrypted_data[1];
  buf_dest_size[3] = decrypted_data[0];
  uint64_t total_dest_size = 0;
  memcpy(&total_dest_size, buf_dest_size, 4);

  dest_buf->AppendBlock(reinterpret_cast<const char*>(&decrypted_data[0] + 4), total_dest_size);
  obj_num_ = 0;
  data_to_be_decrypted_ = nullptr;

  return true;
}

uint32_t PDFCryptoHandlerImpl::EncryptGetSize(
    uint32_t objnum,
    uint32_t version,
    char* src_buf, uint32_t src_size) {
  FILEAPI_UNREFERENCED_PARAMETER(objnum);
  FILEAPI_UNREFERENCED_PARAMETER(version);
  FILEAPI_UNREFERENCED_PARAMETER(src_buf);
  uint32_t encrypted_size = 0;
  encrypted_size = src_size;

  encrypted_size += 4; //add prefix padding
  encrypted_size += (16 - encrypted_size % 16);//add postfix padding

  return encrypted_size;
}

bool PDFCryptoHandlerImpl::EncryptContent(
    uint32_t objnum,
    uint32_t version,
    char* src_buf,
    uint32_t src_size,
    char* dest_buf,
    uint32_t* dest_size) {
  FILEAPI_UNREFERENCED_PARAMETER(objnum);
  FILEAPI_UNREFERENCED_PARAMETER(version);
  if (!pdf_protector_) return false;

  uint64_t content_size_add_pre = src_size + 4;
  std::unique_ptr<char> shared_content_add_pre(new char[content_size_add_pre]);
  char* content_add_pre = shared_content_add_pre.get();
  content_add_pre[3] = ((char*)&src_size)[0];
  content_add_pre[2] = ((char*)&src_size)[1];
  content_add_pre[1] = ((char*)&src_size)[2];
  content_add_pre[0] = ((char*)&src_size)[3];
  memcpy(content_add_pre + 4, src_buf, src_size);

  auto sstream = std::make_shared<std::stringstream>();
  std::shared_ptr<std::iostream> outputIOS = sstream;

  auto output_shared_stream = rmscrypto::api::CreateStreamFromStdStream(outputIOS);
  auto protected_stream = pdf_protector_->CreateProtectedStream(output_shared_stream, content_size_add_pre);
  pdf_protector_->EncryptStream(content_add_pre, content_size_add_pre, protected_stream, true);

  output_shared_stream->Seek(std::ios::beg);
  auto size = output_shared_stream->Size();
  int64_t data_read = output_shared_stream->Read(reinterpret_cast<uint8_t*>(dest_buf), size);
  FILEAPI_UNREFERENCED_PARAMETER(data_read);
  *dest_size = size;

  return true;
}

bool PDFCryptoHandlerImpl::ProgressiveEncryptStart(
    uint32_t objnum,
    uint32_t version,
    uint32_t raw_size) {
  FILEAPI_UNREFERENCED_PARAMETER(objnum);
  FILEAPI_UNREFERENCED_PARAMETER(version);
  if (raw_size > MIN_RAW_SIZE) {
    progressive_start_ = true;
    return true;
  }
  return false;
}

bool PDFCryptoHandlerImpl::ProgressiveEncryptContent(
    uint32_t objnum,
    uint32_t version,
    char* src_buf,
    uint32_t src_size,
    pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  FILEAPI_UNREFERENCED_PARAMETER(objnum);
  FILEAPI_UNREFERENCED_PARAMETER(version);
  FILEAPI_UNREFERENCED_PARAMETER(dest_buf);
  uint64_t content_size_add_pre = 0;
  char* content_add_pre = nullptr;

  if (progressive_start_) {
    content_size_add_pre = src_size + 4;
    std::unique_ptr<char> shared_content_add_pre(new char[content_size_add_pre]);
    content_add_pre = shared_content_add_pre.get();

    content_add_pre[3] = ((char*)&src_size)[0];
    content_add_pre[2] = ((char*)&src_size)[1];
    content_add_pre[1] = ((char*)&src_size)[2];
    content_add_pre[0] = ((char*)&src_size)[3];
    memcpy(content_add_pre + 4, src_buf, src_size);

    auto sstream = std::make_shared<std::stringstream>();
    output_IOS_ = sstream;

    output_shared_stream_ = rmscrypto::api::CreateStreamFromStdStream(output_IOS_);
    shared_protected_stream_ = pdf_protector_->CreateProtectedStream(output_shared_stream_,
                                                                     content_size_add_pre);

    pdf_protector_->EncryptStream(content_add_pre,
                                  content_size_add_pre, shared_protected_stream_, false);
  } else {
    content_size_add_pre = src_size;
    content_add_pre = src_buf;

    pdf_protector_->EncryptStream(content_add_pre,
                                  content_size_add_pre, shared_protected_stream_, false);
  }

  progressive_start_ = false;

  return true;
}

bool PDFCryptoHandlerImpl::ProgressiveEncryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  pdf_protector_->EncryptStream(nullptr, 0, shared_protected_stream_, true);
  output_shared_stream_->Seek(std::ios::beg);
  auto size = output_shared_stream_->Size();

  std::unique_ptr<char> shared_data_read(new char[size]);
  char* buf_data_read = shared_data_read.get();
  int64_t data_read = output_shared_stream_->Read(reinterpret_cast<uint8_t*>(buf_data_read), size);
  FILEAPI_UNREFERENCED_PARAMETER(data_read);
  dest_buf->AppendBlock(buf_data_read, size);

  shared_protected_stream_ = nullptr;
  output_IOS_ = nullptr;
  output_shared_stream_ = nullptr;

  return true;
}

////////////////////////////////////////////////////////////////////////////
/// class PDFSecurityHandlerImpl

PDFSecurityHandlerImpl::PDFSecurityHandlerImpl(
    std::shared_ptr<PDFProtector> pdf_protector,
    const UserContext &usercontext,
    const UnprotectOptions &options,
    std::shared_ptr<std::atomic<bool> > cancelstate)
    : user_context_(usercontext),
      options_(options),
      cancel_state_(cancelstate) {
  pdf_protector_ = pdf_protector;
  crypto_handler_ = nullptr;
}

PDFSecurityHandlerImpl::~PDFSecurityHandlerImpl() {
  crypto_handler_.reset();
  crypto_handler_ = nullptr;
}

bool PDFSecurityHandlerImpl::OnInit(unsigned char *publishing_license,
                                    uint32_t publishing_license_size) {
  std::vector<uint8_t> vec_publishing_license(publishing_license_size);
  memcpy(reinterpret_cast<uint8_t *>(&vec_publishing_license[0]),
         publishing_license, publishing_license_size);

  modernapi::PolicyAcquisitionOptions policyAcquisitionOptions = options_.offlineOnly ?
      modernapi::PolicyAcquisitionOptions::POL_OfflineOnly :
      modernapi::PolicyAcquisitionOptions::POL_None;
  auto cacheMask = modernapi::RESPONSE_CACHE_NOCACHE;
  if (options_.useCache) {
    cacheMask = static_cast<modernapi::ResponseCacheFlags>(
        modernapi::RESPONSE_CACHE_INMEMORY |
        modernapi::RESPONSE_CACHE_ONDISK |
        modernapi::RESPONSE_CACHE_CRYPTED);
  }

  auto policyRequest = modernapi::UserPolicy::Acquire(
      vec_publishing_license,
      user_context_.userId,
      user_context_.authenticationCallback,
      &user_context_.consentCallback,
      policyAcquisitionOptions,
      cacheMask,
      cancel_state_);
  if (policyRequest->Status != modernapi::GetUserPolicyResultStatus::Success) {
    logger::Logger::Error("UserPolicy::Acquire unsuccessful", policyRequest->Status);
    throw exceptions::RMSPDFFileException("The file may be corrupt or the user may have no righs.",
                                          exceptions::RMSPDFFileException::CannotAcquirePolicy);
    return false;
  }

  std::shared_ptr<modernapi::UserPolicy> userPolicy = policyRequest->Policy;
  if (userPolicy.get() == nullptr) {
    logger::Logger::Error("User Policy acquisition failed");
    throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
    return false;
  }

  pdf_protector_->SetUserPolicy(userPolicy);

  return true;
}

std::shared_ptr<pdfobjectmodel::PDFCryptoHandler> PDFSecurityHandlerImpl::CreateCryptoHandler() {
  crypto_handler_ = std::make_shared<PDFCryptoHandlerImpl>(pdf_protector_);
  return crypto_handler_;
}

////////////////////////////////////////////////////////////////////////////
/// class PDFProtector

PDFProtector::PDFProtector(
    const std::string& original_file_path,
    const std::string& original_file_extension,
    std::shared_ptr<std::fstream> inputstream)
    : original_file_extension_(original_file_extension),
      original_file_path_(original_file_path),
      input_stream_(inputstream) {
  pdfobjectmodel::PDFModuleMgr::Initialize();
  pdf_creator_ = pdfobjectmodel::PDFCreator::Create();
  input_wrapper_stream_ = nullptr;
}

PDFProtector::~PDFProtector() {
}

void PDFProtector::SetWrapper(std::shared_ptr<std::fstream> input_wrapper_stream) {
  input_wrapper_stream_ = input_wrapper_stream;
}

void PDFProtector::ProtectWithTemplate(
    const UserContext& usercontext,
    const ProtectWithTemplateOptions& options,
    std::shared_ptr<std::fstream> outputstream,
    std::shared_ptr<std::atomic<bool>> cancelstate) {
  logger::Logger::Hidden("+PDFProtector::ProtectWithTemplate");
  if (!outputstream->is_open()) {
    logger::Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  if (IsProtected()) {
    logger::Logger::Error("File is already protected");
    throw exceptions::RMSPDFFileException("File is already protected",
                                          exceptions::RMSPDFFileException::AlreadyProtected);
    return;
  }

  auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
      options.allowAuditedExtraction, options.cryptoOptions);
  user_policy_ = modernapi::UserPolicy::CreateFromTemplateDescriptor(
      options.templateDescriptor,
      usercontext.userId,
      usercontext.authenticationCallback,
      userPolicyCreationOptions,
      options.signedAppData,
      cancelstate);
  Protect(outputstream);
  logger::Logger::Hidden("-PDFProtector::ProtectWithTemplate");
}

void PDFProtector::ProtectWithCustomRights(
    const UserContext& userContext,
    const ProtectWithCustomRightsOptions& options,
    std::shared_ptr<std::fstream> outputstream,
    std::shared_ptr<std::atomic<bool>> cancelstate) {
  logger::Logger::Hidden("+PDFProtector::ProtectWithCustomRights");
  if (!outputstream->is_open()) {
    logger::Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  if (IsProtected()) {
    logger::Logger::Error("File is already protected");
    throw exceptions::RMSPDFFileException("File is already protected",
                                          exceptions::RMSPDFFileException::AlreadyProtected);
  }

  auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
      options.allowAuditedExtraction, options.cryptoOptions);
  user_policy_ = modernapi::UserPolicy::Create(
      const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
      userContext.userId,
      userContext.authenticationCallback,
      userPolicyCreationOptions,
      cancelstate);
  Protect(outputstream);
  logger::Logger::Hidden("-PDFProtector::ProtectWithCustomRights");
}

UnprotectResult PDFProtector::Unprotect(
    const UserContext& usercontext,
    const UnprotectOptions& options,
    std::shared_ptr<std::fstream> outputstream,
    std::shared_ptr<std::atomic<bool>> cancelstate) {
  logger::Logger::Hidden("+PDFProtector::UnProtect");
  if (!outputstream->is_open()) {
    logger::Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
    return rmscore::fileapi::UnprotectResult::FAILURE;
  }

  std::shared_ptr<std::iostream> input_encrypted_IO = input_stream_;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);

  pdfobjectmodel::PDFSharedStream encrypted_pdf_shared_stream =
      std::make_shared<PDFDataStreamImpl>(input_encrypted);
  std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =
      pdfobjectmodel::PDFWrapperDoc::Create(encrypted_pdf_shared_stream);
  pdfobjectmodel::PDFWrapperDocType wrapper_type = pdf_wrapper_doc->GetWrapperType();
  uint32_t payload_size = pdf_wrapper_doc->GetPayLoadSize();
  std::wstring graphic_filter;
  float version_num = 0;
  pdf_wrapper_doc->GetCryptographicFilter(graphic_filter, version_num);
  if ((wrapper_type != pdfobjectmodel::PDFWrapperDocType::IRMV1 &&
       wrapper_type != pdfobjectmodel::PDFWrapperDocType::IRMV2) ||
      payload_size <= 0 ||
      graphic_filter.compare(PDF_PROTECTOR_WRAPPER_SUBTYPE) != 0) {
    logger::Logger::Error("It is not a valid RMS-protected file.");
    throw exceptions::RMSPDFFileException("It is not a valid RMS-protected file.",
                                          exceptions::RMSPDFFileException::NotValidFile);
    return rmscore::fileapi::UnprotectResult::FAILURE;
  }


  std::shared_ptr<std::iostream> payload_temp_IO;
  if (payload_size > MIN_RAW_SIZE) {
    std::string payload_temp_file_path = original_file_path_;
    payload_temp_file_path += PAYLOAD_TEMP_FILE;

    //create a temp cache file to store the payload instead of storing it in the memory.
    //because the payload may be very large.
    std::shared_ptr<std::fstream> payload_stream(
        new std::fstream(
            payload_temp_file_path,
            std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary),
        [=](std::fstream* file_stream) {
          file_stream->close();
          std::remove(payload_temp_file_path.c_str());
         }
    );

    if (!payload_stream->is_open()) {
      logger::Logger::Error("Failed to create the payload cache file.");
      throw exceptions::RMSStreamException("Failed to create the payload cache file.");
      rmscore::fileapi::UnprotectResult::FAILURE;
    }

    payload_temp_IO = payload_stream;
  } else {
    payload_temp_IO = std::make_shared<std::stringstream>();
  }

  auto payload_temp_std_stream = rmscrypto::api::CreateStreamFromStdStream(payload_temp_IO);
  pdfobjectmodel::PDFSharedStream payload_shared_stream =
      std::make_shared<PDFDataStreamImpl>(payload_temp_std_stream);

  bool bGetPayload = pdf_wrapper_doc->StartGetPayload(payload_shared_stream);
  FILEAPI_UNREFERENCED_PARAMETER(bGetPayload);

  std::shared_ptr<std::iostream> output_decrypted_IO = outputstream;
  auto output_decrypted = rmscrypto::api::CreateStreamFromStdStream(output_decrypted_IO);

  std::string filter_name = PDF_PROTECTOR_FILTER_NAME;

  std::shared_ptr<PDFProtector> shared_pdf_protector(this, [=](PDFProtector* pdf_protector) {
    pdf_protector = nullptr;
    FILEAPI_UNREFERENCED_PARAMETER(pdf_protector);
  });

  auto security_handler = std::make_shared<PDFSecurityHandlerImpl>(
      shared_pdf_protector,
      usercontext,
      options,
      cancelstate);

  pdfobjectmodel::PDFSharedStream decrypted_shared_stream =
      std::make_shared<PDFDataStreamImpl>(output_decrypted);
  pdfobjectmodel::PDFCreatorErr result = pdf_creator_->UnprotectCustomEncryptedFile(
      payload_shared_stream,
      filter_name,
      security_handler,
      decrypted_shared_stream);
  if(pdfobjectmodel::PDFCreatorErr::SUCCESS != result) {
    logger::Logger::Error("Failed to decrypt the file. The file may be corrupted.");
    throw exceptions::RMSPDFFileException("Failed to decrypt the file. The file may be corrupted.",
                                          exceptions::RMSPDFFileException::CorruptFile);
    return rmscore::fileapi::UnprotectResult::FAILURE;
  }

  logger::Logger::Hidden("+PDFProtector::UnProtect");
  return rmscore::fileapi::UnprotectResult::SUCCESS;
}

bool PDFProtector::IsProtected() const {
  logger::Logger::Hidden("+PDFProtector::IsProtected");

  std::shared_ptr<std::iostream> input_encrypted_IO = input_stream_;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);
  pdfobjectmodel::PDFSharedStream encrypted_shared_stream =
      std::make_shared<PDFDataStreamImpl>(input_encrypted);

  std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =
      pdfobjectmodel::PDFWrapperDoc::Create(encrypted_shared_stream);
  pdfobjectmodel::PDFWrapperDocType wrapper_type = pdf_wrapper_doc->GetWrapperType();
  uint32_t payload_size = pdf_wrapper_doc->GetPayLoadSize();
  std::wstring graphic_filter;
  float version_num = 0;
  pdf_wrapper_doc->GetCryptographicFilter(graphic_filter, version_num);
  if ((wrapper_type != pdfobjectmodel::PDFWrapperDocType::IRMV1 &&
       wrapper_type != pdfobjectmodel::PDFWrapperDocType::IRMV2) ||
      (payload_size <= 0) ||
      graphic_filter.compare(PDF_PROTECTOR_WRAPPER_SUBTYPE) != 0) {
    return false;
  }

  logger::Logger::Hidden("The document is protected with rms.");
  logger::Logger::Hidden("-PDFProtector::IsProtected");
  return true;
}

void PDFProtector::Protect(const std::shared_ptr<std::fstream>& outputstream) {
  if (user_policy_.get() == nullptr) {
    logger::Logger::Error("User Policy creation failed");
    throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
  }

  auto publishing_license = user_policy_->SerializedPolicy();

  std::shared_ptr<std::iostream> input_filedata_IO = input_stream_;
  auto input_filedata = rmscrypto::api::CreateStreamFromStdStream(input_filedata_IO);
  pdfobjectmodel::PDFSharedStream pdf_data_shared_stream =
      std::make_shared<PDFDataStreamImpl>(input_filedata);

  uint64_t file_size = pdf_data_shared_stream->GetSize();
  std::shared_ptr<std::iostream> encrypted_temp_IO;
  if (file_size > MIN_RAW_SIZE) {
    std::string encrypted_temp_file_path = original_file_path_;
    encrypted_temp_file_path += ENCRYPTED_TEMP_FILE;

    //create a temp cache file to store the encrypted file instead of storing it in the memory.
    //because the encrypted file may be very large.
    std::shared_ptr<std::fstream> encrypted_temp_stream(
        new std::fstream(
            encrypted_temp_file_path,
            std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary),
        [=](std::fstream* file_stream) {
          file_stream->close();
          std::remove(encrypted_temp_file_path.c_str());
         }
    );

    if (!encrypted_temp_stream->is_open()) {
      logger::Logger::Error("Failed to create the encrypted cache file.");
      throw exceptions::RMSStreamException("Failed to create the encrypted cache file.");
      return;
    }

    encrypted_temp_IO = encrypted_temp_stream;

  } else {
    encrypted_temp_IO = std::make_shared<std::stringstream>();
  }

  auto encrypted_temp_std_stream = rmscrypto::api::CreateStreamFromStdStream(encrypted_temp_IO);
  pdfobjectmodel::PDFSharedStream encrypted_shared_stream =
      std::make_shared<PDFDataStreamImpl>(encrypted_temp_std_stream);

  std::string filter_name = PDF_PROTECTOR_FILTER_NAME;
  std::string cache_file_path = original_file_path_;
  cache_file_path += PROGRESSIVE_ENCRYPT_TEMP_FILE;

  std::shared_ptr<PDFProtector> shared_pdf_protector(this, [=](PDFProtector* pdf_protector) {
    pdf_protector = nullptr;
    FILEAPI_UNREFERENCED_PARAMETER(pdf_protector);
  });

  auto crypto_handler = std::make_shared<PDFCryptoHandlerImpl>(shared_pdf_protector);
  pdfobjectmodel::PDFCreatorErr result = pdf_creator_->CreateCustomEncryptedFile(
      pdf_data_shared_stream,
      cache_file_path,
      filter_name,
      publishing_license,
      crypto_handler,
      encrypted_shared_stream);
  if (pdfobjectmodel::PDFCreatorErr::SUCCESS != result) {
    logger::Logger::Error("Failed to encrypt the file. The file is invalid.");
    throw exceptions::RMSPDFFileException("Failed to encrypt the file. The file is invalid.",
                                          exceptions::RMSPDFFileException::CorruptFile);
    return;
  }

  if (!input_wrapper_stream_) {
    logger::Logger::Error("Not set the input wrapper stream.");
    throw exceptions::RMSInvalidArgumentException("Not set the input wrapper stream.");
    return;
  }
  std::shared_ptr<std::iostream> input_wrapper_IO = input_wrapper_stream_;
  auto input_wrapper = rmscrypto::api::CreateStreamFromStdStream(input_wrapper_IO);
  pdfobjectmodel::PDFSharedStream wrapper_shared_stream =
      std::make_shared<PDFDataStreamImpl>(input_wrapper);

  pdf_wrapper_creator_ = pdfobjectmodel::PDFUnencryptedWrapperCreator::Create(wrapper_shared_stream);
  pdf_wrapper_creator_->SetPayloadInfo(
      PDF_PROTECTOR_WRAPPER_SUBTYPE,
      PDF_PROTECTOR_WRAPPER_FILENAME,
      PDF_PROTECTOR_WRAPPER_DES,
      PDF_PROTECTOR_WRAPPER_VERSION);
  pdf_wrapper_creator_->SetPayLoad(encrypted_shared_stream);

  std::shared_ptr<std::iostream> outputIO = outputstream;
  auto output_wrapper = rmscrypto::api::CreateStreamFromStdStream(outputIO);
  pdfobjectmodel::PDFSharedStream output_wrapper_shared_stream =
      std::make_shared<PDFDataStreamImpl>(output_wrapper);

  std::string stream_cache_file_path = original_file_path_;
  stream_cache_file_path += STREAM_TEMP_FILE;
  bool result_create = pdf_wrapper_creator_->CreateUnencryptedWrapper(
      stream_cache_file_path,
      output_wrapper_shared_stream);
  if (!result_create) {
    logger::Logger::Error("Failed to create PDF IRM V2 file. The wrapper doc may be invalid.");
    throw exceptions::RMSInvalidArgumentException("Failed to create PDF IRM V2 file. The wrapper doc may be invalid.");
    return;
  }
}

std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> PDFProtector::CreateProtectedStream(
    const rmscrypto::api::SharedStream& stream,
    uint64_t contentsize) {
  auto protectionPolicy = user_policy_->GetImpl();
  /*if (protectionPolicy->GetCipherMode() == rmscrypto::api::CipherMode::CIPHER_MODE_ECB)
  {
     // Older versions of the SDK ignored ECB cipher mode when encrypting pfile format.
      protectionPolicy->ReinitilizeCryptoProvider(rmscrypto::api::CipherMode::CIPHER_MODE_CBC4K);
  }*/

  auto cryptoProvider = user_policy_->GetImpl()->GetCryptoProvider();
  block_size_ = cryptoProvider->GetBlockSize();
  // Cache block size to be 512 for cbc512, 4096 for cbc4k and ecb
  uint64_t protectedStreamBlockSize = block_size_ == 512 ? 512 : 4096;
  auto backingStreamImpl = stream->Clone();
  uint64_t contentStartPosition = 0;
  return rmscrypto::api::BlockBasedProtectedStream::Create(
      cryptoProvider,
      backingStreamImpl,
      contentStartPosition,
      contentsize,
      protectedStreamBlockSize);
}

void PDFProtector::EncryptStream(
    const char *plain_buffer,
    const uint32_t buffersize,
    const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& protected_stream,
    const bool finish) {
  if (nullptr == plain_buffer || 0 == buffersize) {
    if(finish) {
      protected_stream->Flush();
    }
    return;
  }

  uint32_t content_size = buffersize;

  uint64_t buf_size_temp = 4096;    //should be a multiple of AES block size (16)
  std::vector<uint8_t> buffer(buf_size_temp);
  uint64_t read_position  = 0;
  uint64_t write_position = 0;
  bool isECB = user_policy_->DoesUseDeprecatedAlgorithms();
  uint64_t total_size = isECB? ((content_size + block_size_ - 1) & ~(block_size_ - 1)) : content_size;

  while (total_size - read_position > 0) {
    uint64_t offset_read  = read_position;
    uint64_t offset_write = write_position;
    uint64_t to_process   = std::min(buf_size_temp, total_size - read_position);
    read_position  += to_process;
    write_position += to_process;

    memcpy(reinterpret_cast<char *>(&buffer[0]), plain_buffer + offset_read, to_process);

    auto written = protected_stream->WriteAsync(
        buffer.data(),
        to_process,
        offset_write,
        std::launch::deferred).get();

    if (written != static_cast<int64_t>(to_process)) {
      throw exceptions::RMSStreamException("Error while writing data");
    }
  }

  if (finish) {
    protected_stream->Flush();
  }
}

void PDFProtector::DecryptStream(
    const rmscrypto::api::SharedStream& outputIOS,
    const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& protected_stream,
    const uint64_t original_content_size) {
  uint64_t buf_size_temp = 4096;    //should be a multiple of AES block size (16)
  std::vector<uint8_t> buffer(buf_size_temp);
  uint64_t read_position  = 0;
  uint64_t write_position = 0;
  uint64_t total_size = protected_stream->Size();
  while (total_size - read_position > 0) {
    uint64_t offset_read  = read_position;
    uint64_t offset_write = write_position;
    uint64_t to_process   = std::min(buf_size_temp, total_size - read_position);
    uint64_t original_remaining = std::min(buf_size_temp, original_content_size - read_position);
    read_position  += to_process;
    write_position += to_process;

    auto read = protected_stream->ReadAsync(
        &buffer[0],
        to_process,
        offset_read,
        std::launch::deferred).get();
    if (read == 0) {
      break;
    }

    outputIOS->Seek(offset_write);
    outputIOS->Write(buffer.data(), original_remaining);
  }
  outputIOS->Flush();
}

modernapi::UserPolicyCreationOptions PDFProtector::ConvertToUserPolicyCreationOptions(
    const bool& allow_audited_extraction,
    CryptoOptions crypto_options) {
  auto userPolicyCreationOptions = allow_audited_extraction ?
      modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
      modernapi::UserPolicyCreationOptions::USER_None;
  if (crypto_options == CryptoOptions::AES128_ECB) {
    userPolicyCreationOptions = static_cast<modernapi::UserPolicyCreationOptions>(
        userPolicyCreationOptions |
        modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
  }
  return userPolicyCreationOptions;
}

void PDFProtector::SetUserPolicy(std::shared_ptr<modernapi::UserPolicy> userpolicy) {
  user_policy_ = userpolicy;
}

} // namespace fileapi
} // namespace rmscore
