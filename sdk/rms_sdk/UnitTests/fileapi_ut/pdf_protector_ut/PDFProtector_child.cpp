/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PDFProtector_child.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include "BlockBasedProtectedStream.h"
#include "RMSExceptions.h"
#include "Common/CommonTypes.h"
#include "Core/ProtectionPolicy.h"

namespace rmscore {
namespace fileapi {

////////////////////////////////////////////////////////////////////////////
/// class PDFProtector_unit
PDFCryptoHandler_child::PDFCryptoHandler_child(std::shared_ptr<PDFProtector_unit> pPDFProtector_unit) {
  m_pPDFProtector_unit = pPDFProtector_unit;
  progressive_start_ = false;
  data_to_be_decrypted_ = nullptr;
  obj_num_ = 0;

  shared_protected_stream_ = nullptr;
  output_IOS_ = nullptr;
  output_shared_stream_ = nullptr;
}

PDFCryptoHandler_child::~PDFCryptoHandler_child() {
  m_pPDFProtector_unit = nullptr;
}

uint32_t PDFCryptoHandler_child::DecryptGetSize(uint32_t src_size) {
  return src_size;
}

void PDFCryptoHandler_child::DecryptStart(uint32_t objnum, uint32_t gennum) {
  obj_num_ = objnum;
  data_to_be_decrypted_ = std::make_shared<std::stringstream>();
}

bool PDFCryptoHandler_child::DecryptStream(char* src_buf, uint32_t src_size, pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  data_to_be_decrypted_->write(src_buf, src_size);
  return true;
}

bool PDFCryptoHandler_child::DecryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  data_to_be_decrypted_->seekg(0, std::ios::end);
  uint64_t count = data_to_be_decrypted_->tellg();
  if (count <= 0) {
    obj_num_ = 0;
    data_to_be_decrypted_.reset();
    data_to_be_decrypted_ = nullptr;
    return true;
  }

  std::shared_ptr<std::iostream> protectedIOS = data_to_be_decrypted_;
  auto input_shared_stream = rmscrypto::api::CreateStreamFromStdStream(protectedIOS);
  auto protected_stream = m_pPDFProtector_unit->CreateProtectedStream(input_shared_stream, count);

  std::shared_ptr<std::stringstream> outputSS = std::make_shared<std::stringstream>();
  std::shared_ptr<std::iostream> outputIOS = outputSS;
  auto output_shared_stream = rmscrypto::api::CreateStreamFromStdStream(protectedIOS);

  m_pPDFProtector_unit->DecryptStream(output_shared_stream, protected_stream, count);

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
  data_to_be_decrypted_.reset();
  data_to_be_decrypted_ = nullptr;

  return true;
}

uint32_t PDFCryptoHandler_child::EncryptGetSize(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size) {
  uint32_t encrypted_size = 0;
  encrypted_size = src_size;

  encrypted_size += 4; //add prefix padding
  encrypted_size += (16 - encrypted_size % 16);//add postfix padding

  return encrypted_size;
}

bool PDFCryptoHandler_child::EncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, char* dest_buf, uint32_t* dest_size) {
  if (!m_pPDFProtector_unit) return false;

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
  auto protected_stream = m_pPDFProtector_unit->CreateProtectedStream(output_shared_stream, content_size_add_pre);
  m_pPDFProtector_unit->EncryptStream(content_add_pre, content_size_add_pre, protected_stream, true);

  output_shared_stream->Seek(std::ios::beg);
  auto size = output_shared_stream->Size();
  int64_t data_read = output_shared_stream->Read(reinterpret_cast<uint8_t*>(dest_buf), size);
  *dest_size = size;

  return true;
}

bool PDFCryptoHandler_child::ProgressiveEncryptStart(uint32_t objnum, uint32_t version, uint32_t raw_size) {
  if (raw_size > MIN_RAW_SIZE) {
    progressive_start_ = true;
    return true;
  }
  return false;
}

bool PDFCryptoHandler_child::ProgressiveEncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, pdfobjectmodel::PDFBinaryBuf* dest_buf) {
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
    shared_protected_stream_ = m_pPDFProtector_unit->CreateProtectedStream(output_shared_stream_, content_size_add_pre);
  } else {
    content_size_add_pre = src_size;
    content_add_pre = src_buf;
  }

  m_pPDFProtector_unit->EncryptStream(content_add_pre, content_size_add_pre, shared_protected_stream_, false);

  progressive_start_ = false;

  return true;
}

bool PDFCryptoHandler_child::ProgressiveEncryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf) {
  m_pPDFProtector_unit->EncryptStream(nullptr, 0, shared_protected_stream_, true);

  output_shared_stream_->Seek(std::ios::beg);
  auto size = output_shared_stream_->Size();

  std::unique_ptr<char> shared_data_read(new char[size]);
  char* buf_data_read = shared_data_read.get();
  int64_t data_read = output_shared_stream_->Read(reinterpret_cast<uint8_t*>(buf_data_read), size);

  dest_buf->AppendBlock(buf_data_read, size);

  shared_protected_stream_.reset();
  output_IOS_.reset();
  output_shared_stream_.reset();

  shared_protected_stream_ = nullptr;
  output_IOS_ = nullptr;
  output_shared_stream_ = nullptr;

  return true;
}

////////////////////////////////////////////////////////////////////////////
/// class PDFSecurityHandler_child

PDFSecurityHandler_child::PDFSecurityHandler_child(
    std::shared_ptr<PDFProtector_unit> pPDFProtector_unit,
    const UserContext &userContext,
    const UnprotectOptions &options,
    std::shared_ptr<std::atomic<bool> > cancelState)
    : user_context_(userContext),
      options_(options),
      cancel_state_(cancelState) {
  m_pPDFProtector_unit = pPDFProtector_unit;
  crypto_handler_ = nullptr;
}

PDFSecurityHandler_child::~PDFSecurityHandler_child() {
  crypto_handler_.reset();
  crypto_handler_ = nullptr;
}

bool PDFSecurityHandler_child::OnInit(unsigned char *publishing_license, uint32_t publishing_license_size) {
  std::vector<uint8_t> vec_publishing_license(publishing_license_size);
  memcpy(reinterpret_cast<uint8_t *>(&vec_publishing_license[0]), publishing_license, publishing_license_size);

  modernapi::PolicyAcquisitionOptions policyAcquisitionOptions = options_.offlineOnly?
      modernapi::PolicyAcquisitionOptions::POL_OfflineOnly :
      modernapi::PolicyAcquisitionOptions::POL_None;
  auto cacheMask = modernapi::RESPONSE_CACHE_NOCACHE;
  if (options_.useCache) {
    cacheMask = static_cast<modernapi::ResponseCacheFlags>(
        modernapi::RESPONSE_CACHE_INMEMORY|
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
    throw exceptions::RMSPDFFileException("The file may be corrupt or the user may have no righs.",
                                          exceptions::RMSPDFFileException::CannotAcquirePolicy);
    return false;
  }

  std::shared_ptr<modernapi::UserPolicy> userPolicy = policyRequest->Policy;
  if (userPolicy.get() == nullptr) {
    throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
    return false;
  }

  bool bIsIssuedToOwner = userPolicy->IsIssuedToOwner();
  if (!bIsIssuedToOwner) {
    throw exceptions::RMSException(
        exceptions::RMSException::ExceptionTypes::LogicError,
        exceptions::RMSException::ErrorTypes::RightsError,
        "Only the owner has the right to unprotect the document.");
  }

  m_pPDFProtector_unit->SetUserPolicy(userPolicy);

  return true;
}

std::shared_ptr<pdfobjectmodel::PDFCryptoHandler> PDFSecurityHandler_child::CreateCryptoHandler() {
  crypto_handler_ = std::make_shared<PDFCryptoHandler_child>(m_pPDFProtector_unit);
  return crypto_handler_;
}

////////////////////////////////////////////////////////////////////////////
/// class PDFProtector_unit

PDFProtector_unit::PDFProtector_unit(
    const std::string& originalFilePath,
    const std::string& originalFileExtension,
    std::shared_ptr<std::fstream> input_stream)
    : original_file_extension_(originalFileExtension),
      input_stream_(input_stream),
      original_file_path_(originalFilePath) {
  pdfobjectmodel::PDFModuleMgr::Initialize();
  pdf_creator_ = pdfobjectmodel::PDFCreator::Create();
  input_wrapper_stream_ = nullptr;
}

PDFProtector_unit::~PDFProtector_unit() {
}

void PDFProtector_unit::SetWrapper(std::shared_ptr<std::fstream> inputWrapperStream) {
  input_wrapper_stream_ = inputWrapperStream;
}

void PDFProtector_unit::ProtectWithTemplate(
    const UserContext& userContext,
    const ProtectWithTemplateOptions& options,
    std::shared_ptr<std::fstream> output_stream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  // Logger::Hidden("+PDFProtector_unit::ProtectWithTemplate");
  if (!output_stream->is_open()) {
    // Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  if (IsProtected()) {
    // Logger::Error("File is already protected");
    throw exceptions::RMSPDFFileException("File is already protected",
                                          exceptions::RMSPDFFileException::AlreadyProtected);
    return;
  }

  auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
      options.allowAuditedExtraction, options.cryptoOptions);
  user_policy_ = modernapi::UserPolicy::CreateFromTemplateDescriptor(
      options.templateDescriptor,
      userContext.userId,
      userContext.authenticationCallback,
      userPolicyCreationOptions,
      options.signedAppData,
      cancelState);
  Protect(output_stream);
  // Logger::Hidden("-PDFProtector_unit::ProtectWithTemplate");
}

void PDFProtector_unit::ProtectWithCustomRights(
    const UserContext& userContext,
    const ProtectWithCustomRightsOptions& options,
    std::shared_ptr<std::fstream> output_stream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  // Logger::Hidden("+PDFProtector_unit::ProtectWithCustomRights");
  if (!output_stream->is_open()) {
    //  Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  if (IsProtected()) {
    //  Logger::Error("File is already protected");
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
      cancelState);
  Protect(output_stream);
  //  Logger::Hidden("-PDFProtector_unit::ProtectWithCustomRights");
}

UnprotectResult PDFProtector_unit::Unprotect(
    const UserContext& userContext,
    const UnprotectOptions& options,
    std::shared_ptr<std::fstream> output_stream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  // Logger::Hidden("+PDFProtector_unit::UnProtect");
  if (!output_stream->is_open()) {
    // Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
    return rmscore::fileapi::UnprotectResult::FAILURE;
  }

  std::shared_ptr<std::iostream> input_encrypted_IO = input_stream_;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);

  std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =  pdfobjectmodel::PDFWrapperDoc::Create(input_encrypted);
  uint32_t wrapper_type = pdf_wrapper_doc->GetWrapperType();
  uint32_t payload_size = pdf_wrapper_doc->GetPayLoadSize();
  std::wstring graphic_filter;
  float version_num = 0;
  pdf_wrapper_doc->GetCryptographicFilter(graphic_filter, version_num);
  if((wrapper_type != PDFWRAPPERDOC_TYPE_IRMV1 && wrapper_type != PDFWRAPPERDOC_TYPE_IRMV2) ||
     (payload_size <= 0) ||
     graphic_filter.compare(PDF_PROTECTOR_WRAPPER_SUBTYPE) != 0) {
    //  Logger::Error("It is not a valid RMS-protected file.");
    throw exceptions::RMSPDFFileException("It is not a valid RMS-protected file.",
                                          exceptions::RMSPDFFileException::NotValidFile);
    return rmscore::fileapi::UnprotectResult::FAILURE;
  }

  auto payloadSS = std::make_shared<std::stringstream>();
  std::shared_ptr<std::iostream> payloadIOS = payloadSS;
  auto output_payload = rmscrypto::api::CreateStreamFromStdStream(payloadIOS);
  bool bGetPayload = pdf_wrapper_doc->StartGetPayload(output_payload);

  std::shared_ptr<std::iostream> output_decrypted_IO = output_stream;
  auto output_decrypted = rmscrypto::api::CreateStreamFromStdStream(output_decrypted_IO);

  std::string filter_name = PDF_PROTECTOR_FILTER_NAME;

  std::shared_ptr<PDFProtector_unit> shared_pdf_protector(this, [=](PDFProtector_unit* pdf_protector) {
    pdf_protector = nullptr;
  });
  auto security_hander = std::make_shared<PDFSecurityHandler_child>(shared_pdf_protector, userContext, options, cancelState);

  uint32_t result = pdf_creator_->UnprotectCustomEncryptedFile(
      output_payload,
      filter_name,
      security_hander,
      output_decrypted);
  if (PDFCREATOR_ERR_SUCCESS != result) {
    // Logger::Error("Failed to decrypt the file. The file may be corrupted.");
    throw exceptions::RMSPDFFileException("Failed to decrypt the file. The file may be corrupted.",
                                          exceptions::RMSPDFFileException::CorruptFile);
    return rmscore::fileapi::UnprotectResult::FAILURE;
  }

  //Logger::Hidden("+PDFProtector_unit::UnProtect");
  return rmscore::fileapi::UnprotectResult::SUCCESS;
}

bool PDFProtector_unit::IsProtected() const {
  // Logger::Hidden("+PDFProtector_unit::IsProtected");

  std::shared_ptr<std::iostream> input_encrypted_IO = input_stream_;
  auto input_encrypted = rmscrypto::api::CreateStreamFromStdStream(input_encrypted_IO);

  std::unique_ptr<pdfobjectmodel::PDFWrapperDoc> pdf_wrapper_doc =  pdfobjectmodel::PDFWrapperDoc::Create(input_encrypted);
  uint32_t wrapper_type = pdf_wrapper_doc->GetWrapperType();
  uint32_t payload_size = pdf_wrapper_doc->GetPayLoadSize();
  std::wstring graphic_filter;
  float version_num = 0;
  pdf_wrapper_doc->GetCryptographicFilter(graphic_filter, version_num);
  if((wrapper_type != PDFWRAPPERDOC_TYPE_IRMV1 && wrapper_type != PDFWRAPPERDOC_TYPE_IRMV2) ||
     (payload_size <= 0) ||
     graphic_filter.compare(PDF_PROTECTOR_WRAPPER_SUBTYPE) != 0) {
    return false;
  }

  // Logger::Hidden("The document is protected with rms.");
  // Logger::Hidden("-PDFProtector_unit::IsProtected");
  return true;
}

void PDFProtector_unit::Protect(const std::shared_ptr<std::fstream>& output_stream) {
  if (user_policy_.get() == nullptr) {
    //   Logger::Error("User Policy creation failed");
    throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
  }

  auto publishing_license = user_policy_->SerializedPolicy();

  auto encryptedSS = std::make_shared<std::stringstream>();
  std::shared_ptr<std::iostream> encryptedIOS = encryptedSS;
  auto output_encrypted = rmscrypto::api::CreateStreamFromStdStream(encryptedIOS);

  std::string filter_name = PDF_PROTECTOR_FILTER_NAME;

  std::shared_ptr<PDFProtector_unit> shared_pdf_protector(this, [=](PDFProtector_unit* pdf_protector) {
    pdf_protector = nullptr;
  });

  auto crypto_hander = std::make_shared<PDFCryptoHandler_child>(shared_pdf_protector);
  uint32_t result = pdf_creator_->CreateCustomEncryptedFile(
      original_file_path_,
      filter_name,
      publishing_license,
      crypto_hander,
      output_encrypted);
  if (PDFCREATOR_ERR_SUCCESS != result) {
    // Logger::Error("Failed to encrypt the file. The file may be corrupted.");
    throw exceptions::RMSPDFFileException("Failed to encrypt the file. The file may be corrupted.",
                                          exceptions::RMSPDFFileException::CorruptFile);
    return;
  }

  if (!input_wrapper_stream_) {
    // Logger::Error("Not set the input wrapper stream.");
    throw exceptions::RMSInvalidArgumentException("Not set the input wrapper stream.");
    return;
  }
  std::shared_ptr<std::iostream> input_wrapper_IO = input_wrapper_stream_;
  auto input_wrapper = rmscrypto::api::CreateStreamFromStdStream(input_wrapper_IO);
  pdf_wrapper_creator_ = pdfobjectmodel::PDFUnencryptedWrapperCreator::Create(input_wrapper);
  pdf_wrapper_creator_->SetPayloadInfo(
      PDF_PROTECTOR_WRAPPER_SUBTYPE,
      PDF_PROTECTOR_WRAPPER_FILENAME,
      PDF_PROTECTOR_WRAPPER_DES,
      PDF_PROTECTOR_WRAPPER_VERSION);
  pdf_wrapper_creator_->SetPayLoad(output_encrypted);

  std::shared_ptr<std::iostream> outputIO = output_stream;
  auto output_wrapper = rmscrypto::api::CreateStreamFromStdStream(outputIO);
  bool create_result = pdf_wrapper_creator_->CreateUnencryptedWrapper(output_wrapper);
  if (!create_result) {
    // Logger::Error("Failed to create PDF IRM V2 file. The wrapper doc may be invalid.");
    throw exceptions::RMSInvalidArgumentException("Failed to create PDF IRM V2 file. The wrapper doc may be invalid.");
    return;
  }
}

std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> PDFProtector_unit::CreateProtectedStream(
    const rmscrypto::api::SharedStream& stream,
    uint64_t content_size) {
  auto protectionPolicy = user_policy_->GetImpl();

  auto cryptoProvider = user_policy_->GetImpl()->GetCryptoProvider();
  block_size_ = cryptoProvider->GetBlockSize();
  // Cache block size to be 512 for cbc512, 4096 for cbc4k and ecb
  uint64_t protectedStreamBlockSize = block_size_ == 512 ? 512 : 4096;
  auto backingStream_child = stream->Clone();
  uint64_t contentStartPosition = 0;
  return rmscrypto::api::BlockBasedProtectedStream::Create(
      cryptoProvider,
      backingStream_child,
      contentStartPosition,
      content_size,
      protectedStreamBlockSize);
}

void PDFProtector_unit::EncryptStream(
    char* pBuffer,
    uint32_t bufferSize,
    const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
    bool bFinish) {
  if (nullptr == pBuffer || 0 == bufferSize) {
    if (bFinish) {
      pStream->Flush();
    }
    return;
  }

  uint32_t content_size = bufferSize;

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

    memcpy(reinterpret_cast<char *>(&buffer[0]), pBuffer + offset_read, to_process);

    auto written = pStream->WriteAsync(
        buffer.data(),
        to_process,
        offset_write,
        std::launch::deferred).get();

    if (written != to_process) {
      throw exceptions::RMSStreamException("Error while writing data");
    }
  }

  if(bFinish) {
    pStream->Flush();
  }
}

void PDFProtector_unit::DecryptStream(
    const rmscrypto::api::SharedStream& outputIOS,
    const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
    uint64_t originalContentSize) {
  uint64_t buf_size_temp = 4096;    //should be a multiple of AES block size (16)
  std::vector<uint8_t> buffer(buf_size_temp);
  uint64_t read_position  = 0;
  uint64_t write_position = 0;
  uint64_t total_size = pStream->Size();
  while (total_size - read_position > 0) {
    uint64_t offset_read  = read_position;
    uint64_t offset_write = write_position;
    uint64_t to_process   = std::min(buf_size_temp, total_size - read_position);
    uint64_t original_remaining = std::min(buf_size_temp, originalContentSize - read_position);
    read_position  += to_process;
    write_position += to_process;

    auto read = pStream->ReadAsync(
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

modernapi::UserPolicyCreationOptions PDFProtector_unit::ConvertToUserPolicyCreationOptions(
    const bool& allowAuditedExtraction,
    CryptoOptions cryptoOptions) {
  auto userPolicyCreationOptions = allowAuditedExtraction ?
      modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
      modernapi::UserPolicyCreationOptions::USER_None;
  if (cryptoOptions == CryptoOptions::AES128_ECB) {
    userPolicyCreationOptions = static_cast<modernapi::UserPolicyCreationOptions>(
        userPolicyCreationOptions |
        modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
  }
  return userPolicyCreationOptions;
}

void PDFProtector_unit::SetUserPolicy(std::shared_ptr<modernapi::UserPolicy> userPolicy) {
  user_policy_ = userPolicy;
}

} // namespace fileapi
} // namespace rmscore