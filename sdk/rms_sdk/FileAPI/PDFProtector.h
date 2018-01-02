/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PDFPROTECTOR_H_
#define RMS_SDK_FILE_API_PDFPROTECTOR_H_

#include "Protector.h"
#include "BlockBasedProtectedStream.h"
#include "FileAPIStructures.h"
#include "UserPolicy.h"
#include "pdf_object_model.h"
#include "CryptoAPI.h"

namespace pdfobjectmodel = rmscore::pdfobjectmodel;

namespace rmscore {
namespace fileapi {

/**
 * @brief It determines whether to encrypt PDF string\stream objects progressively or not.
 * If the raw size is larger than MIN_RAW_SIZE and PDFCryptoHandlerImpl::ProgressiveEncryptStart
 * returns true, foxit core will encrypt PDF string\stream objects progressively.
 */
#define MIN_RAW_SIZE 64 * 1024 * 1024
#define PROGRESSIVE_ENCRYPT_TEMP_FILE ".RMS.PE.temp"
#define ENCRYPTED_TEMP_FILE ".encrypted.temp"
#define PAYLOAD_TEMP_FILE ".payload.temp"
#define STREAM_TEMP_FILE ".stream.temp"

/**
 * @brief The implementation class of interface class PDFDataStream.
 * It is used to access to the stream data.
 * It is implemented by PDF protect, and it is invoked by PDF object model to access to the stream data.
 */
class PDFDataStreamImpl : public pdfobjectmodel::PDFDataStream {
 public:
  explicit PDFDataStreamImpl(rmscrypto::api::SharedStream ioStream);
  virtual ~PDFDataStreamImpl();

  virtual void Release();

  virtual uint64_t GetSize();

  virtual bool IsEOF();

  virtual uint64_t GetPosition();

  virtual bool ReadBlock(void* buffer, uint64_t offset, uint64_t size);

  virtual uint64_t ReadBlock(void* buffer, uint64_t size);

  virtual bool WriteBlock(const void* buffer, uint64_t offset, uint64_t size);

  virtual bool Flush();

 private:
  rmscrypto::api::SharedStream shared_io_stream_;
};

/**
 * @brief The wrapper info used to create the unencrypted wrapper document
 * according to "Microsoft IRM protection for PDF Spec-v2".
 */
#define PDF_PROTECTOR_FILTER_NAME       "MicrosoftIRMServices"
#define PDF_PROTECTOR_WRAPPER_SUBTYPE   L"MicrosoftIRMServices"
#define PDF_PROTECTOR_WRAPPER_FILENAME  L"MicrosoftIRMServices Protected PDF.pdf"
#define PDF_PROTECTOR_WRAPPER_DES       L"This embedded file is encrypted using MicrosoftIRMServices filter"
#define PDF_PROTECTOR_WRAPPER_VERSION   2

/**
 * @brief The PDF protector supports the protection of PDF IRM V2 and the un-protection of
 * PDF IRM V2 and PDF IRM V1, according to "Microsoft IRM protection for PDF Spec-v2".
 * The PDF protector is on the base of PDF object model.
 * The interface header file of PDF object model is PDFObjectModel.h.
 * We have to invoke pdfobjectmodel::PDFModuleMgr::Initialize to initialize the PDF object model first.
 */
class PDFProtector : public ProtectorWithWrapper {
 public:
  /**
   * @brief Constructs the protector to protect or unprotect a PDF document.
   * We have to invoke pdfobjectmodel::PDFModuleMgr::Initialize to initialize the PDF object model first.
   * @param[in] original_file_path      The input PDF file path to be protected.
   * @param[in] original_file_extension The input file extension name.
   * @param[in] inputstream            The protector will read the original PDF document data from the input file stream.
   * @return void.
   */
  PDFProtector(
      const std::string& original_file_path,
      const std::string& original_file_extension,
      std::shared_ptr<std::fstream> inputstream);

  ~PDFProtector();

  /**
   * @brief Sets the PDF wrapper document to wrap up the encrypted PDF document.
   * If the PDF wrapper document is invalid, ProtectWithTemplate will fail and throw an exception.
   * @param[in] input_wrapper_stream      The protector will read the PDF wrapper document data from the input file stream.
   * @return void.
   */
  void SetWrapper(std::shared_ptr<std::fstream> input_wrapper_stream) override;

  /**
   * @brief The protector protects the PDF document with the official rights policy template.
   * First, the protector invokes pdfobjectmodel::PDFCreator::CreateCustomEncryptedFile to create
   * custom encrypted PDF document(be opposed to standard encryption like password encryption or certificate encryption).
   * Second, the protector invokes pdfobjectmodel::PDFUnencryptedWrapperCreator relevant interfaces to create final
   * PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] usercontext    The input user context.
   * @param[in] options        The input options.
   * @param[in] outputstream   It receives the final PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] cancelstate    It is used to check the process abandon.
   * @return void.
   */
  void ProtectWithTemplate(
      const UserContext& usercontext,
      const ProtectWithTemplateOptions& options,
      std::shared_ptr<std::fstream> outputstream,
      std::shared_ptr<std::atomic<bool>> cancelstate) override;

  /**
   * @brief The protector protects the PDF document with the custom rights policy.
   * First, the protector invokes pdfobjectmodel::PDFCreator::CreateCustomEncryptedFile to create
   * custom encrypted PDF document(be opposed to standard encryption like password encryption or certificate encryption).
   * Second, the protector invokes pdfobjectmodel::PDFUnencryptedWrapperCreator relevant interfaces to create final
   * PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] usercontext    The input user context.
   * @param[in] options        The input options.
   * @param[in] outputstream   It receives the final PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] cancelstate    It is used to check the process abandon.
   * @return void.
   */
  void ProtectWithCustomRights(
      const UserContext& usercontext,
      const ProtectWithCustomRightsOptions& options,
      std::shared_ptr<std::fstream> outputstream,
      std::shared_ptr<std::atomic<bool>> cancelstate) override;

  /**
   * @brief The protector unprotects the PDF document. It supports PDF IRM V2&V1 format.
   * First, the protector invokes pdfobjectmodel::PDFWrapperDoc relevant interfaces to check whether
   * the input document is rms-protected.
   * Second, the protector invokes pdfobjectmodel::PDFWrapperDoc::StartGetPayload
   * to obtain the encrypted PDF document.
   * Third, the protector invokes pdfobjectmodel::PDFCreator::UnprotectCustomEncryptedFile to
   * decrypt the encrypted PDF document.
   * @param[in] usercontext    The input user context.
   * @param[in] options        The input options.
   * @param[in] outputstream   It receives the final PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] cancelstate    It is used to check the process abandon.
   * @return The unprotection result.
   */
  UnprotectResult Unprotect(
      const UserContext& usercontext,
      const UnprotectOptions& options,
      std::shared_ptr<std::fstream> outputstream,
      std::shared_ptr<std::atomic<bool>> cancelstate) override;

  /**
   * @brief The protector invokes pdfobjectmodel::PDFWrapperDoc relevant interfaces to
   * check whether the input PDF document is protected or not.
   * @return true if the document is protected, otherwise false.
   */
  bool IsProtected() const override;

  std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
      const rmscrypto::api::SharedStream& stream,
      uint64_t contentsize);

  void EncryptStream(
      const char* plain_buffer,
      const uint32_t buffersize,
      const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& protected_stream,
      const bool finish);

  void DecryptStream(
      const rmscrypto::api::SharedStream& outputIOS,
      const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& protected_stream,
      const uint64_t original_content_size);

  void SetUserPolicy(std::shared_ptr<modernapi::UserPolicy> userpolicy);

 private:
  void Protect(const std::shared_ptr<std::fstream>& outputstream);
  modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptions(
      const bool& allow_audited_extraction,
      CryptoOptions crypto_options);

  std::string original_file_extension_;
  std::string original_file_path_;
  std::shared_ptr<std::fstream> input_stream_;
  uint32_t block_size_;
  std::shared_ptr<modernapi::UserPolicy> user_policy_;

  std::shared_ptr<std::fstream> input_wrapper_stream_;
  std::unique_ptr<pdfobjectmodel::PDFCreator> pdf_creator_;
  std::unique_ptr<pdfobjectmodel::PDFUnencryptedWrapperCreator> pdf_wrapper_creator_;
};

/**
 * @brief The implementation class of interface class PDFCryptoHandler defined by PDF object model.
 * Please refer to comments of PDFCryptoHandler.
 * The interface header file of PDF object model is PDFObjectModel.h.
 */
class PDFCryptoHandlerImpl : public pdfobjectmodel::PDFCryptoHandler {
 public:
  explicit PDFCryptoHandlerImpl(std::shared_ptr<PDFProtector> pdf_protector);
  virtual ~PDFCryptoHandlerImpl();

  virtual uint32_t DecryptGetSize(uint32_t src_size);

  virtual void DecryptStart(uint32_t objnum, uint32_t gennum);

  virtual bool DecryptStream(
      char* src_buf,
      uint32_t src_size,
      pdfobjectmodel::PDFBinaryBuf* dest_buf);

  virtual bool DecryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf);

  virtual uint32_t EncryptGetSize(
      uint32_t objnum,
      uint32_t version,
      char* src_buf,
      uint32_t src_size);

  virtual bool EncryptContent(
      uint32_t objnum,
      uint32_t version,
      char* src_buf,
      uint32_t src_size,
      char* dest_buf,
      uint32_t* dest_size);

  virtual bool ProgressiveEncryptStart(
      uint32_t objnum,
      uint32_t version,
      uint32_t raw_size);

  virtual bool ProgressiveEncryptContent(
      uint32_t objnum,
      uint32_t version,
      char* src_buf,
      uint32_t src_size,
      pdfobjectmodel::PDFBinaryBuf* dest_buf);

  virtual bool ProgressiveEncryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf);

 private:
  std::shared_ptr<PDFProtector> pdf_protector_;

  //for decryption
  std::shared_ptr<std::stringstream> data_to_be_decrypted_;
  uint32_t obj_num_;

  //for encryption
  bool progressive_start_;
  std::shared_ptr<std::iostream> output_IOS_;
  rmscrypto::api::SharedStream output_shared_stream_;
  std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> shared_protected_stream_;
};

/**
 * @brief The implementation class of interface class PDFSecurityHandler defined by PDF object model.
 * Please refer to comments of PDFSecurityHandler.
 * The interface header file of PDF object model is PDFObjectModel.h.
 */
class PDFSecurityHandlerImpl : public pdfobjectmodel::PDFSecurityHandler {
 public:
  PDFSecurityHandlerImpl(
      std::shared_ptr<PDFProtector> pdf_protector,
      const UserContext& usercontext,
      const UnprotectOptions& options,
      std::shared_ptr<std::atomic<bool>> cancelstate);
  virtual ~PDFSecurityHandlerImpl();

  virtual bool OnInit(unsigned char* publishing_license,
                      uint32_t publishing_license_size);

  virtual std::shared_ptr<pdfobjectmodel::PDFCryptoHandler> CreateCryptoHandler();

 private:
  std::shared_ptr<PDFProtector> pdf_protector_;
  std::shared_ptr<PDFCryptoHandlerImpl> crypto_handler_;
  UserContext user_context_;
  UnprotectOptions options_;
  std::shared_ptr<std::atomic<bool>> cancel_state_;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PDFPROTECTOR_H_

