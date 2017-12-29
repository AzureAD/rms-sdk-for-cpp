/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMSSDK_PDFOBJECTMODEL_H_
#define RMSSDK_PDFOBJECTMODEL_H_

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace rmscore {
namespace pdfobjectmodel {

//#define RMS_LIBRARY_STATIC
//#define CDECL_CALLING_CONVENTION

#ifdef RMS_LIBRARY_STATIC
  #define DLL_PUBLIC_RMS
  #ifdef CDECL_CALLING_CONVENTION
    #define RMS_CALLING_CONVENTION __cdecl
  #else
    #define RMS_CALLING_CONVENTION
  #endif
#elif defined _WIN32 || defined __CYGWIN__
  #define RMS_CALLING_CONVENTION
  #ifdef RMS_LIBRARY
    #ifdef __GNUC__
      #define DLL_PUBLIC_RMS __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC_RMS __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC_RMS __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC_RMS __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #define RMS_CALLING_CONVENTION
  #if __GNUC__ >= 4
    #define DLL_PUBLIC_RMS __attribute__ ((visibility ("default")))
    #define DLL_LOCAL_RMS  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC_RMS
    #define DLL_LOCAL_RMS
  #endif
#endif

/**
 * @brief The PDF object model is designed to parse PDF document and creat PDF document.
 * It is on the base of Foxit PDF core.
 * We can create custom encrypted PDF file(be opposed to standard encryption like
 * password encryption or certificate encryption) and decrypt it through PDFCreator
 * revelant interfaces, according to "Microsoft IRM protection for PDF Spec-v2".
 * We can wrap up the encrypted PDF file with the wrapper document through PDFUnencryptedWrapperCreator
 * relevant interfaces, according to "Microsoft IRM protection for PDF Spec-v2".
 * We can checks the encrypted PDF document wrapped up with wrapper document through PDFWrapperDoc
 * relevant interfaces.
 * The other interface classes are assistant classes, like PDFBinaryBuf, PDFCryptoHandler, PDFSecurityHandler.
 *
 * IMPORTANT !!! IMPORTANT !!! IMPORTANT !!!
 * Saying important thing three times. ^-^
 * Firstly, we MUST initialize the PDF object model through PDFModuleMgr::Initialize.
 */
class PDFModuleMgr {
 public:
  DLL_PUBLIC_RMS
  static bool RMS_CALLING_CONVENTION Initialize();

  virtual ~PDFModuleMgr(){}

 protected:
  PDFModuleMgr(){}
};

/**
 * @brief It is used in PDFCryptoHandler to deal with the binary buf.
 * It is implemented by PDF object model, and it is invoked by PDF protector.
 */
class PDFBinaryBuf {
 public:
  /**
   * @brief Appends some buffer to the existing buffer.
   * @param[in] buffer_pointer    The input buffer to be appended.
   * @param[in] size              The input buffer size.
   * @return true for success, otherwise false.
   */
  virtual bool AppendBlock(const void* buffer_pointer, uint32_t size) = 0;
};

/**
 * @brief It is used to access to the stream data.
 * It is implemented by PDF protect, and it is invoked by PDF object model to access to the stream data.
 */
class PDFDataStream {
 public:
  virtual void Release() = 0;

  virtual uint64_t GetSize() = 0;

  virtual bool IsEOF() = 0;

  virtual uint64_t GetPosition() = 0;

  virtual bool ReadBlock(void* buffer, uint64_t offset, uint64_t size) = 0;

  virtual uint64_t ReadBlock(void* buffer, uint64_t size) = 0;

  virtual bool WriteBlock(const void* buffer, uint64_t offset, uint64_t size) = 0;

  virtual bool Flush() = 0;
};

typedef std::shared_ptr<PDFDataStream> PDFSharedStream;

/**
 * @brief The type definitions of PDF wrapper doc.
 */
enum class PDFWrapperDocType{
  UNKNOWN = -1, /** Unknown. */
  NORMAL,   /** Normal document. */
  IRMV1,    /** For IRM V1 wrapper document. */
  IRMV2,    /** For IRM V2 wrapper document. */
  DIGITALLY_SIGNED, /** PDF document is digitally signed. */
  NOT_IRM_ENCRYPTED,	/** PDF document is encrypted, but not encrypted by IRM V1 or V2. */
  NOT_COMPATIBLE_WITH_IRM, /** Error happens when our PDF library parse the PDF document. */
};

/**
 * @brief It is used to checks the encrypted PDF document wrapped up with wrapper document.
 * We can get the wrapper type, payload size, payload content and so on.
 * It is implemented by PDF object model, and it is invoked by PDF protector.
 */
class PDFWrapperDoc {
 public:
  /**
   * @brief Creates the PDFWrapperDoc instance.
   * @param[in] input_stream    The input wrapper document.
   * @return The PDFWrapperDoc instance.
   */
  DLL_PUBLIC_RMS
  static std::unique_ptr<PDFWrapperDoc> RMS_CALLING_CONVENTION Create(PDFSharedStream input_stream);

  /**
   * @brief Gets the wrapper type.
   * @return Refers to definition of PDFWrapperDocType.
   */
  virtual PDFWrapperDocType GetWrapperType() const = 0;

  /**
   * @brief Gets the cryptographic filter name and version.
   * @param[out] graphic_filter  It receives the cryptographic filter name.
   * @param[out] version_num         It receives the cryptographic filter version.
   * @return true for success, otherwise false.
   */
  virtual bool GetCryptographicFilter(std::wstring& graphic_filter, float &version_num) const = 0;

  /**
   * @brief Gets the payload size.
   * @return The payload size.
   */
  virtual uint32_t GetPayLoadSize() const = 0;

  /**
   * @brief Gets the payload file name.
   * @param[out] file_name  It receives the payload file name.
   * @return true for success, otherwise false.
   */
  virtual bool GetPayloadFileName(std::wstring& file_name) const = 0;

  /**
   * @brief Gets the payload content.
   * @param[out] output_stream  It receives the payload content.
   * @return true for success, otherwise false.
   */
  virtual bool StartGetPayload(PDFSharedStream output_stream) = 0;

  virtual ~PDFWrapperDoc(){}

 protected:
  PDFWrapperDoc(){}
};

/**
 * @brief It is used to wrap up the encrypted PDF file with the wrapper document.
 * First, we have to set the payload info and the payload content. Then invoke
 * CreateUnencryptedWrapper to output the final wrapped up PDF document.
 * It is implemented by PDF object model, and it is invoked by PDF protector.
 */
class PDFUnencryptedWrapperCreator {
 public:
  /**
   * @brief Creates the PDFUnencryptedWrapperCreator instance.
   * @param[in] wrapper_doc_stream    The input wrapper document.
   * @return The PDFUnencryptedWrapperCreator instance.
   */
  DLL_PUBLIC_RMS
  static std::unique_ptr<PDFUnencryptedWrapperCreator> RMS_CALLING_CONVENTION Create(
      PDFSharedStream wrapper_doc_stream);

  /**
   * @brief Sets the payload info.
   * @param[in] sub_type     The name of the cryptographic filter used to encrypt the encrypted payload document.
   * @param[in] file_name    The file name for encrypted payload document.
   * @param[in] description  Description text for the embedded encrypted payload document.
   * @param[in] version_num  The version number of the cryptographic filter.
   * @return void.
   */
  virtual void SetPayloadInfo(
      const std::wstring& sub_type,
      const std::wstring& file_name,
      const std::wstring& description,
      float version_num) = 0;

  /**
   * @brief Sets the payload content.
   * @param[in] input_stream   The input payload content.
   * @return void.
   */
  virtual void SetPayLoad(PDFSharedStream input_stream) = 0;

  /**
   * @brief Writes the wrapper doc to a custom file access.
   * @param[in] cache_file_path The input cache file path if the payload content is large.
   * Set it empty if you don't want to create the cache file.
   * @param[out] output_stream  It receives the wrapper doc.
   * @return true for success, otherwise false.
   */
  virtual bool CreateUnencryptedWrapper(const std::string& cache_file_path, PDFSharedStream output_stream) = 0;

  virtual ~PDFUnencryptedWrapperCreator(){}

 protected:
  PDFUnencryptedWrapperCreator(){}
};

/**
 * @brief This class is responsible for PDF data encryption and decryption.
 * It is implemented by PDF protector, and set it to CreateCustomEncryptedFile
 * or returned by PDFSecurityHandler::CreateCryptoHandler.
 * It will be invoked by PDF object model to pass the PDF data to PDF protector,
 * then PDF protector process the PDF data(encryption or decryption), and then
 * PDF protector feeds back the data to PDF object model.
 */
class PDFCryptoHandler {
 public:

  /**
   * @brief The PDF object model passes the encrypted data size to PDF protector and the
   * PDF protector feeds back the decrypted data size so that the PDF object model can prepare
   * the memory to receive the decrypted data.
   */
  virtual uint32_t DecryptGetSize(uint32_t src_size) = 0;

  virtual void DecryptStart(uint32_t objnum, uint32_t gennum) = 0;

  virtual bool DecryptStream(
      char* src_buf,
      uint32_t src_size,
      PDFBinaryBuf* dest_buf) = 0;

  virtual bool DecryptFinish(PDFBinaryBuf* dest_buf) = 0;

  /**
   * @brief The PDF object model passes the plain data size to PDF protector and the
   * PDF protector feeds back the encrypted data size so that the PDF object model can prepare
   * the memory to receive the encrypted data.
   *
   * IMPORTANT !!! IMPORTANT !!! IMPORTANT !!!
   * EncryptGetSize and EncryptContent are one group dealing with the one time encryption.
   * ProgressiveEncryptStart, ProgressiveEncryptContent and ProgressiveEncryptFinish are one
   * group dealing with the progressive encryption if the data size is large.
   * The return value of ProgressiveEncryptStart determines which mode is adopted. If it returns
   * true, the progressive mode is adopted, otherwise the one time mode is adopted.
   */
  virtual uint32_t EncryptGetSize(
      uint32_t objnum,
      uint32_t version,
      char* src_buf,
      uint32_t src_size) = 0;

  virtual bool EncryptContent(
      uint32_t objnum,
      uint32_t version,
      char* src_buf,
      uint32_t src_size,
      char* dest_buf,
      uint32_t* dest_size) = 0;

  virtual bool ProgressiveEncryptStart(
      uint32_t objnum,
      uint32_t version,
      uint32_t raw_size) = 0;

  virtual bool ProgressiveEncryptContent(
      uint32_t objnum,
      uint32_t version,
      char* src_buf,
      uint32_t src_size,
      PDFBinaryBuf* dest_buf) = 0;

  virtual bool ProgressiveEncryptFinish(PDFBinaryBuf* dest_buf) = 0;
};

/**
 * @brief This class is responsible for PDF security handler.
 * It is implemented by PDF protector, and set it to UnprotectCustomEncryptedFile.
 * It will be invoked by PDF object model to pass the publishing license to PDF protector
 * to check the validity and to create crypto hander for PDF data decryption.
 */
class PDFSecurityHandler {
 public:
  /**
   * @brief The PDF protector checks the publishing license validity here. If false
   * is returned, UnprotectCustomEncryptedFile will fail.
   */
  virtual bool OnInit(unsigned char* publishing_license,
                      uint32_t publishing_license_size) = 0;

  virtual std::shared_ptr<PDFCryptoHandler> CreateCryptoHandler() = 0;
};

/**
 * @brief The error code definitions for PDFCreator.
 */
enum class PDFCreatorErr{
  SUCCESS = 0, /*No error.*/
  FILE, /*File error: not found or could not be opened.*/
  FORMAT, /*Format error: not a PDF or corrupted.*/
  SECURITY, /*This document is encrypted by some other security handler.*/
  CREATOR, /*The PDF creator failed to create PDF data.*/
  UNKNOWN_ERR, /*Unknown error.*/
};

/**
 * @brief It is used to create custom encrypted PDF file(be opposed to standard encryption like
 * password encryption or certificate encryption) and decrypt it, according to
 * "Microsoft IRM protection for PDF Spec-v2".
 * It is implemented by PDF object model, and it is invoked by PDF protector.
 */
class PDFCreator {
 public:
  DLL_PUBLIC_RMS
  static std::unique_ptr<PDFCreator> RMS_CALLING_CONVENTION Create();

  /**
   * @brief Creates custom encrypted PDF file(be opposed to standard encryption like
   * password encryption or certificate encryption).
   * @param[in] inputFileData      The input file path to be encrypted.
   * @param[in] cache_file_path    The input cache file path when encrypting.
   * @param[in] filter_name        The filter name, according to "Microsoft IRM protection for PDF Spec-v2".
   * @param[in] publishing_license The publishing license stored in PDF file, according to "Microsoft IRM protection for PDF Spec-v2".
   * @param[in] crypto_handler      The crypto handler that is responsible for PDF data encryption.
   * @param[out] outputIOS         It receives the encrypted PDF documnet.
   * @return PDFCreatorErr::SUCCESS for success, otherwise the other error code.
   */
  virtual PDFCreatorErr CreateCustomEncryptedFile(
      PDFSharedStream inputFileData,
      const std::string& cache_file_path,
      const std::string& filter_name,
      const std::vector<unsigned char>& publishing_license,
      std::shared_ptr<PDFCryptoHandler> crypto_handler,
      PDFSharedStream outputIOS) = 0;

  /**
   * @brief Decrypts the custom encrypted PDF file(be opposed to standard encryption like
   * password encryption or certificate encryption).
   * @param[in] inputIOS          The input custom encrypted PDF document.
   * @param[in] filter_name        The filter name, according to "Microsoft IRM protection for PDF Spec-v2".
   * @param[in] security_handler    The securty handler used to check the validity and to create crypto hander for PDF data decryption.
   * @param[out] outputIOS        It receives the decrypted PDF documnet.
   * @return PDFCreatorErr::SUCCESS for success, otherwise the other error code.
   */
  virtual PDFCreatorErr UnprotectCustomEncryptedFile(
      PDFSharedStream inputIOS,
      const std::string& filter_name,
      std::shared_ptr<PDFSecurityHandler> security_handler,
      PDFSharedStream outputIOS) = 0;

  virtual ~PDFCreator(){}

 protected:
  PDFCreator(){}
};

/**
 * @brief .
 */
class PDFMetadataCreator {
 public:
  DLL_PUBLIC_RMS
  static std::unique_ptr<PDFMetadataCreator> RMS_CALLING_CONVENTION Create(
      PDFSharedStream input_pdf_document_stream);

  virtual std::string GetPDFVersion() = 0;

  virtual void GetEntries(std::map<std::string, std::string>& entries) = 0;

  virtual void RemoveEntry(std::string key) = 0;

  virtual void AddEntry(const std::string& key, const std::string& value) = 0;

  virtual bool Save(PDFSharedStream output_pdf_document_stream) = 0;

  virtual ~PDFMetadataCreator(){}

 protected:
  PDFMetadataCreator(){}
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMSSDK_PDFOBJECTMODEL_H_

