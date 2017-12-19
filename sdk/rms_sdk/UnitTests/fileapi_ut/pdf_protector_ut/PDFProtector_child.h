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
#include "UserPolicy.h"
#include "PDFObjectModel/pdf_object_model.h"
#include "CryptoAPI.h"

namespace rmscore{
namespace fileapi {

namespace pdfobjectmodel = rmscore::pdfobjectmodel;

#define MIN_RAW_SIZE 64 * 1024 * 1024
#define PROGRESSIVE_ENCRYPT_TEMP_FILE ".RMS.PE.temp"

/**
 * @brief The implementaion class of interface class PDFDataStream.
 * It is used to access to the stream data.
 * It is implemented by PDF protect, and it is invoked by PDF object model to access to the stream data.
 */
class FDFDataStreamImpl_unit : public pdfobjectmodel::PDFDataStream {
 public:
  explicit FDFDataStreamImpl_unit(rmscrypto::api::SharedStream ioStream);
  virtual ~FDFDataStreamImpl_unit();

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
class PDFProtector_unit : public ProtectorWithWrapper {
 public:
  /**
   * @brief Constructs the protector to protect or unprotect a PDF document.
   * We have to invoke pdfobjectmodel::PDFModuleMgr::Initialize to initialize the PDF object model first.
   * @param[in] originalFilePath      The input PDF file path to be protected.
   * @param[in] originalFileExtension The input file extension name.
   * @param[in] input_stream           The protector will read the original PDF document data from the input file stream.
   * @return void.
   */
  PDFProtector_unit(
      const std::string& originalFilePath,
      const std::string& originalFileExtension,
      std::shared_ptr<std::fstream> input_stream);

  ~PDFProtector_unit();

  /**
   * @brief Sets the PDF wrapper document to wrap up the encrypted PDF document.
   * If the PDF wrapper document is invalid, ProtectWithTemplate will fail and throw an exception.
   * @param[in] inputWrapperStream      The protector will read the PDF wrapper document data from the input file stream.
   * @return void.
   */
  void SetWrapper(std::shared_ptr<std::fstream> inputWrapperStream) override;

  /**
   * @brief The protector protects the PDF document with the official rights policy template.
   * First, the protector invokes pdfobjectmodel::PDFCreator::CreateCustomEncryptedFile to create
   * custom encrypted PDF document(be opposed to standard encryption like password encryption or certificate encryption).
   * Second, the protector invokes pdfobjectmodel::PDFUnencryptedWrapperCreator relevant interfaces to create final
   * PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] userContext    The input user context.
   * @param[in] options        The input options.
   * @param[in] output_stream   It receives the final PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] cancelState    It is used to check the process abandon.
   * @return void.
   */
  void ProtectWithTemplate(
      const UserContext& userContext,
      const ProtectWithTemplateOptions& options,
      std::shared_ptr<std::fstream> output_stream,
      std::shared_ptr<std::atomic<bool>> cancelState) override;

  /**
   * @brief The protector protects the PDF document with the custom rights policy.
   * First, the protector invokes pdfobjectmodel::PDFCreator::CreateCustomEncryptedFile to create
   * custom encrypted PDF document(be opposed to standard encryption like password encryption or certificate encryption).
   * Second, the protector invokes pdfobjectmodel::PDFUnencryptedWrapperCreator relevant interfaces to create final
   * PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] userContext    The input user context.
   * @param[in] options        The input options.
   * @param[in] output_stream   It receives the final PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] cancelState    It is used to check the process abandon.
   * @return void.
   */
  void ProtectWithCustomRights(
      const UserContext& userContext,
      const ProtectWithCustomRightsOptions& options,
      std::shared_ptr<std::fstream> output_stream,
      std::shared_ptr<std::atomic<bool>> cancelState) override;

  /**
   * @brief The protector unprotects the PDF document. It supports PDF IRM V2&V1 format.
   * First, the protector invokes pdfobjectmodel::PDFWrapperDoc relevant interfaces to check whether
   * the input document is rms-protected.
   * Second, the protector invokes pdfobjectmodel::PDFWrapperDoc::StartGetPayload
   * to obtain the encrypted PDF document.
   * Third, the protector invokes pdfobjectmodel::PDFCreator::UnprotectCustomEncryptedFile to
   * decrypt the encrypted PDF document.
   * @param[in] userContext    The input user context.
   * @param[in] options        The input options.
   * @param[in] output_stream   It receives the final PDF IRM V2 document wrapped up with unencrypted wrapper document.
   * @param[in] cancelState    It is used to check the process abandon.
   * @return The unprotection result.
   */
  UnprotectResult Unprotect(
      const UserContext& userContext,
      const UnprotectOptions& options,
      std::shared_ptr<std::fstream> output_stream,
      std::shared_ptr<std::atomic<bool>> cancelState) override;

  /**
   * @brief The protector invokes pdfobjectmodel::PDFWrapperDoc relevant interfaces to
   * check whether the input PDF document is protected or not.
   * @return true if the document is protected, otherwise false.
   */
  bool IsProtected() const override;

  std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
      const rmscrypto::api::SharedStream& stream,
      uint64_t content_size);

  void EncryptStream(
      char* pBuffer,
      uint32_t bufferSize,
      const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
      bool bFinish);

  void DecryptStream(
      const rmscrypto::api::SharedStream& outputIOS,
      const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
      uint64_t originalContentSize);

  void SetUserPolicy(std::shared_ptr<modernapi::UserPolicy> userPolicy);

 private:
  void Protect(const std::shared_ptr<std::fstream>& output_stream);
  modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptions(
      const bool& allowAuditedExtraction,
      CryptoOptions cryptoOptions);

  std::string original_file_path_;
  std::string original_file_extension_;
  std::shared_ptr<std::fstream> input_stream_;
  uint32_t block_size_;
  std::shared_ptr<modernapi::UserPolicy> user_policy_;

  std::shared_ptr<std::fstream> input_wrapper_stream_;
  std::unique_ptr<pdfobjectmodel::PDFCreator> pdf_creator_;
  std::unique_ptr<pdfobjectmodel::PDFUnencryptedWrapperCreator> pdf_wrapper_creator_;
};

/**
 * @brief The _childementaion class of interface class PDFCryptoHandler defined by PDF object model.
 * Please refer to comments of PDFCryptoHandler.
 * The interface header file of PDF object model is PDFObjectModel.h.
 */
class PDFCryptoHandler_child : public rmscore::pdfobjectmodel::PDFCryptoHandler {
 public:
  explicit PDFCryptoHandler_child(std::shared_ptr<PDFProtector_unit> pPDFProtector_unit);
  virtual ~PDFCryptoHandler_child();

  virtual uint32_t DecryptGetSize(uint32_t src_size);
  virtual void DecryptStart(uint32_t objnum, uint32_t gennum);

  virtual bool DecryptStream(char* src_buf, uint32_t src_size, pdfobjectmodel::PDFBinaryBuf* dest_buf);

  virtual bool DecryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf);

  virtual uint32_t EncryptGetSize(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size);

  virtual bool EncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, char* dest_buf, uint32_t* dest_size);

  virtual bool ProgressiveEncryptStart(uint32_t objnum, uint32_t version, uint32_t raw_size);

  virtual bool ProgressiveEncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, pdfobjectmodel::PDFBinaryBuf* dest_buf);

  virtual bool ProgressiveEncryptFinish(pdfobjectmodel::PDFBinaryBuf* dest_buf);

 private:
  std::shared_ptr<PDFProtector_unit> m_pPDFProtector_unit;

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
 * @brief The _childementaion class of interface class PDFSecurityHandler defined by PDF object model.
 * Please refer to comments of PDFSecurityHandler.
 * The interface header file of PDF object model is PDFObjectModel.h.
 */
class PDFSecurityHandler_child : public rmscore::pdfobjectmodel::PDFSecurityHandler {
 public:
  PDFSecurityHandler_child(
      std::shared_ptr<PDFProtector_unit> pPDFProtector_unit,
      const UserContext& userContext,
      const UnprotectOptions& options,
      std::shared_ptr<std::atomic<bool>> cancelState);
  virtual ~PDFSecurityHandler_child();

  virtual bool OnInit(unsigned char* publishing_license, uint32_t publishing_license_size);

  virtual std::shared_ptr<pdfobjectmodel::PDFCryptoHandler> CreateCryptoHandler();

 private:
  std::shared_ptr<PDFProtector_unit> m_pPDFProtector_unit;
  std::shared_ptr<pdfobjectmodel::PDFCryptoHandler> crypto_handler_;
  UserContext user_context_;
  UnprotectOptions options_;
  std::shared_ptr<std::atomic<bool>> cancel_state_;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_PDFPROTECTOR_CHILD_H_

