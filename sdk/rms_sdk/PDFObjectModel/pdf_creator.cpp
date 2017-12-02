#include "pdf_creator.h"

#include <cstdio>

#include "pdf_module_mgr.h"
#include "basic.h"

namespace rmscore {
namespace pdfobjectmodel {

//////////////////////////////////////////////////////////////////////////
//class CustomCryptoHandler

CustomCryptoHandler::CustomCryptoHandler(std::shared_ptr<PDFCryptoHandler> pdf_crypto_handler) {
  pdf_crypto_handler_ = pdf_crypto_handler;
}

CustomCryptoHandler::~CustomCryptoHandler() {
  pdf_crypto_handler_ = nullptr;
}

FX_BOOL CustomCryptoHandler::Init(CPDF_Dictionary* encryption_dictionary,
                                  CPDF_SecurityHandler* security_handler) {
  FX_UNREFERENCED_PARAMETER(encryption_dictionary);
  FX_UNREFERENCED_PARAMETER(security_handler);
  return true;
}

FX_DWORD CustomCryptoHandler::DecryptGetSize(FX_DWORD src_size) {
  if (pdf_crypto_handler_) {
    uint32_t size = 0;
    size = pdf_crypto_handler_->DecryptGetSize(src_size);
    return size;
  }
  return 0;
}

FX_LPVOID CustomCryptoHandler::DecryptStart(FX_DWORD objnum, FX_DWORD gennum) {
  if (pdf_crypto_handler_) {
    pdf_crypto_handler_->DecryptStart(objnum, gennum);
  }
  return nullptr;
}

FX_BOOL CustomCryptoHandler::DecryptStream(
    FX_LPVOID context,
    FX_LPCBYTE src_buf,
    FX_DWORD src_size,
    CFX_BinaryBuf& dest_buf) {
  FX_UNREFERENCED_PARAMETER(context);
  if (pdf_crypto_handler_) {
     PDFBinaryBufImpl bin_buf(&dest_buf);
     return pdf_crypto_handler_->DecryptStream((char*)src_buf, src_size, &bin_buf);
  }
  return false;
}

FX_BOOL CustomCryptoHandler::DecryptFinish(FX_LPVOID context,
                                           CFX_BinaryBuf& dest_buf) {
  FX_UNREFERENCED_PARAMETER(context);
  if (pdf_crypto_handler_) {
    PDFBinaryBufImpl bin_buf(&dest_buf);
    return pdf_crypto_handler_->DecryptFinish(&bin_buf);
  }
  return false;
}

FX_DWORD CustomCryptoHandler::EncryptGetSize(
    FX_DWORD objnum,
    FX_DWORD version,
    FX_LPCBYTE src_buf,
    FX_DWORD src_size) {
  if (pdf_crypto_handler_) {
    return pdf_crypto_handler_->EncryptGetSize(objnum, version, (char*)src_buf, src_size);
  }
  return 0;
}

FX_BOOL CustomCryptoHandler::EncryptContent(
    FX_DWORD objnum,
    FX_DWORD version,
    FX_LPCBYTE src_buf,
    FX_DWORD src_size,
    FX_LPBYTE dest_buf,
    FX_DWORD& dest_size) {
  if (pdf_crypto_handler_) {
    return pdf_crypto_handler_->EncryptContent(
        objnum,
        version,
        (char*)src_buf,
        src_size,
        (char*)dest_buf,
        (uint32_t*)(&dest_size));
  }
  return false;
}

FX_BOOL CustomCryptoHandler::ProgressiveEncryptStart(
    FX_DWORD objnum,
    FX_DWORD version,
    FX_DWORD raw_size,
    FX_BOOL flate_encode) {
  FX_UNREFERENCED_PARAMETER(flate_encode);
  if (pdf_crypto_handler_) {
    return pdf_crypto_handler_->ProgressiveEncryptStart(objnum, version, raw_size);
  }
  return false;
}

FX_BOOL CustomCryptoHandler::ProgressiveEncryptContent(
    FX_INT32 objnum,
    FX_DWORD version,
    FX_LPCBYTE src_buf,
    FX_DWORD src_size,
    CFX_BinaryBuf& dest_buf) {
  if (pdf_crypto_handler_) {

    PDFBinaryBufImpl bin_buf(&dest_buf);
    return pdf_crypto_handler_->ProgressiveEncryptContent(
        objnum,
        version,
        (char*)src_buf,
        src_size, &bin_buf);
  }

  return false;
}

FX_BOOL CustomCryptoHandler:: ProgressiveEncryptFinish(CFX_BinaryBuf& dest_buf) {
  if (pdf_crypto_handler_) {

    PDFBinaryBufImpl bin_buf(&dest_buf);
    return pdf_crypto_handler_->ProgressiveEncryptFinish(&bin_buf);
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////
//class CustomProgressiveEncryptHandler

CustomProgressiveEncryptHandler::CustomProgressiveEncryptHandler(CFX_WideString temp_path) {
  temp_path_ = temp_path;
  crypto_handler_ = nullptr;
  obj_number_ = 0;
  version_number_ = 0;
}

CustomProgressiveEncryptHandler::~CustomProgressiveEncryptHandler() {
  crypto_handler_ = nullptr;
}


IFX_FileStream* CustomProgressiveEncryptHandler::GetTempFile() {
  CFX_WideString path = temp_path_;
  path += PROGRESSIVE_ENCRYPT_TEMP_FILE;
  IFX_FileStream*	file_stream = FX_CreateFileStream(path, FX_FILEMODE_Truncate);
  return file_stream;
}


void CustomProgressiveEncryptHandler::ReleaseTempFile(IFX_FileStream* file_stream) {
  if (file_stream) {
    file_stream->Release();

    CFX_WideString path = temp_path_;
    path += PROGRESSIVE_ENCRYPT_TEMP_FILE;

    CFX_ByteString utf8_path = path.UTF8Encode();
    std::remove((const char *)(FX_LPCSTR)utf8_path);
  }
}


FX_BOOL CustomProgressiveEncryptHandler::SetCryptoHandler(CPDF_CryptoHandler* crypto_handler) {
  crypto_handler_ = (CustomCryptoHandler*)crypto_handler;
  return true;
}


FX_DWORD CustomProgressiveEncryptHandler::EncryptGetSize(
    FX_DWORD objnum,
    FX_DWORD gennum,
    FX_LPCBYTE src_buf,
    FX_DWORD src_size) {
  FX_DWORD encrypted_size = 0;
  if (crypto_handler_) {
    encrypted_size = crypto_handler_->EncryptGetSize(objnum, gennum, src_buf, src_size);
  }
  return encrypted_size;
}


FX_LPVOID CustomProgressiveEncryptHandler::EncryptStart(
    FX_DWORD objnum,
    FX_DWORD gennum,
    FX_DWORD raw_size,
    FX_BOOL flate_encode) {
  bool result = false;
  obj_number_ = objnum;
  version_number_ = gennum;
  if (crypto_handler_) {
    result = (crypto_handler_->ProgressiveEncryptStart(
        obj_number_,
        version_number_,
        raw_size,
        flate_encode) !=0);
  }

  if (result) {
    return crypto_handler_;
  }

  return NULL;
}


FX_BOOL CustomProgressiveEncryptHandler::EncryptStream(
    FX_LPVOID context,
    FX_LPCBYTE src_buf,
    FX_DWORD src_size,
    IFX_FileStream* dest_file_stream) {
  FX_UNREFERENCED_PARAMETER(context);
  if (crypto_handler_) {
    CFX_BinaryBuf dest_buf;
    FX_BOOL result = crypto_handler_->ProgressiveEncryptContent(
        obj_number_,
        version_number_,
        src_buf,
        src_size,
        dest_buf);
     if (result) {
       result = dest_file_stream->WriteBlock(dest_buf.GetBuffer(), dest_buf.GetSize());
      }
    return result;
  }
  return false;
}


FX_BOOL CustomProgressiveEncryptHandler::EncryptFinish(FX_LPVOID context,
                                                       IFX_FileStream* dest_file_stream) {
  FX_UNREFERENCED_PARAMETER(context);
  FX_BOOL result = false;
  if (crypto_handler_) {
    CFX_BinaryBuf dest_buf;
    result = crypto_handler_->ProgressiveEncryptFinish(dest_buf);
    if (result) {
      result = dest_file_stream->WriteBlock(dest_buf.GetBuffer(), dest_buf.GetSize());
    }
  }
  obj_number_ = 0;
  version_number_ = 0;
  return result;
}


FX_BOOL CustomProgressiveEncryptHandler::UpdateFilter(CPDF_Dictionary* pdf_dictionary) {
  FX_UNREFERENCED_PARAMETER(pdf_dictionary);
  return true;
}

//////////////////////////////////////////////////////////////////////////
//class CustomSecurityHandler

CustomSecurityHandler::CustomSecurityHandler(
    std::shared_ptr<PDFSecurityHandler> pdf_security_handler) {
  pdf_security_handler_ = pdf_security_handler;
  encrypt_metadata_ = true;
}

CustomSecurityHandler::~CustomSecurityHandler() {
  pdf_security_handler_ = nullptr;
}

FX_BOOL CustomSecurityHandler::OnInit(CPDF_Parser* pdf_parser,
                                      CPDF_Dictionary* encryption_dictionary) {
  FX_UNREFERENCED_PARAMETER(pdf_parser);

  if (encryption_dictionary->KeyExist("EncryptMetadata")) {
    encrypt_metadata_ = (encryption_dictionary->GetBoolean("EncryptMetadata") != 0);
  }

  CFX_ByteString publishing_license_bytestring =
      encryption_dictionary->GetString("PublishingLicense");

  FX_INT32 irm_version = encryption_dictionary->GetInteger("MicrosoftIRMVersion");
  if(irm_version <= 1) {
    CFX_ByteString publishing_license_decoded_bytestring;
    CFX_Base64Decoder base64_decoder;
    uint32_t length_decoded = base64_decoder.Decode(publishing_license_bytestring,
                                                    publishing_license_decoded_bytestring);

    FX_LPBYTE dest_buf = nullptr;
    FX_DWORD dest_size = 0;
    FlateDecode((FX_LPCBYTE)publishing_license_decoded_bytestring,
                length_decoded, dest_buf, dest_size);

    publishing_license_bytestring.Empty();
    if (dest_buf[0] == 0xef && dest_buf[1] == 0xbb && dest_buf[2] == 0xbf) {
      publishing_license_bytestring.Load(dest_buf, dest_size);
      CFX_WideString publishing_license_widestring =
          publishing_license_bytestring.UTF8Decode();

      CFX_ByteString publishing_license_temp;
      uint8_t size_of_wchar = sizeof(wchar_t);
      if (size_of_wchar == 4) {
        FX_LPBYTE pUCS2 = nullptr;
        FX_DWORD output_ucs_2_length = 0;;
        utility::UCS4ToUCS2(publishing_license_widestring, &pUCS2, &output_ucs_2_length);
        publishing_license_temp.Load(pUCS2, output_ucs_2_length);
        if (pUCS2 != nullptr) {
          delete [] pUCS2;
          pUCS2 = nullptr;
        }
      } else {
        publishing_license_temp.Load(
            ((FX_LPCBYTE)(FX_LPCWSTR)publishing_license_widestring),
            publishing_license_widestring.GetLength() * sizeof(wchar_t));
      }

      if (publishing_license_temp.GetAt(0) == 0xff &&
          publishing_license_temp.GetAt(1) == 0xfe) {
        publishing_license_temp.Delete(0, 2);
      }

      publishing_license_bytestring = publishing_license_temp;
    } else {
      FX_DWORD sizeUTF8 = dest_size + 3;
      std::unique_ptr<FX_BYTE> sharedUTF8Bytes(new FX_BYTE[sizeUTF8]);
      FX_LPBYTE pUTF8Bytes = sharedUTF8Bytes.get();
      memset(pUTF8Bytes, 0, sizeof(FX_BYTE) * (sizeUTF8));
      pUTF8Bytes[0] = 0xef;
      pUTF8Bytes[1] = 0xbb;
      pUTF8Bytes[2] = 0xbf;

      utility::UTF16ToUTF8(
          (UTF16*)dest_buf,
          (UTF16*)(dest_buf + dest_size),
          pUTF8Bytes + 3,
          pUTF8Bytes + dest_size + 3);

      publishing_license_bytestring.Load(pUTF8Bytes, sizeUTF8);
    }

    FX_Free(dest_buf);
  }

  if (pdf_security_handler_) {
    uint32_t publishing_license_size = publishing_license_bytestring.GetLength();
    unsigned char* publishing_license = (FX_LPBYTE)(FX_LPCBYTE)publishing_license_bytestring;
    bool bInit = pdf_security_handler_->OnInit(publishing_license, publishing_license_size);
    return bInit;
  }

  return true;
}

FX_DWORD CustomSecurityHandler::GetPermissions() {
  return 0;
}

//PDF Object Model is for PDF protection and unprotection.
//The upper layer RMS system controls the permission.
FX_BOOL CustomSecurityHandler::IsOwner() {
  return true;
}

//This interface is for PDF standard encryption\decryption algorithm.
//RMS has its own algorithm to deal with the data in CustomCryptoHandler.
FX_BOOL CustomSecurityHandler::GetCryptInfo(int& cipher, FX_LPCBYTE& buffer, int& keylen) {
  FX_UNREFERENCED_PARAMETER(cipher);
  FX_UNREFERENCED_PARAMETER(buffer);
  FX_UNREFERENCED_PARAMETER(keylen);
  return true;
}

FX_BOOL CustomSecurityHandler::IsMetadataEncrypted() {
  return encrypt_metadata_;
}

CPDF_CryptoHandler* CustomSecurityHandler::CreateCryptoHandler() {
  CPDF_CryptoHandler* core_pdf_crypto_handler = nullptr;
  std::shared_ptr<PDFCryptoHandler> pdf_crypto_handler = nullptr;
  if (pdf_security_handler_) {
    pdf_crypto_handler = pdf_security_handler_->CreateCryptoHandler();
  }
  if (!pdf_crypto_handler) {
    core_pdf_crypto_handler= new CPDF_StandardCryptoHandler;
  } else {
    core_pdf_crypto_handler = new CustomCryptoHandler(pdf_crypto_handler);
  }

  //foxit pdf core takes over core_pdf_crypto_handler
  return core_pdf_crypto_handler;
}

//////////////////////////////////////////////////////////////////////////
//class PDFCreatorImpl

std::unique_ptr<PDFCreator> PDFCreator::Create() {
  std::unique_ptr<PDFCreator> pdf_creator(new PDFCreatorImpl());
  return pdf_creator;
}

PDFCreatorImpl::PDFCreatorImpl() {

}

PDFCreatorImpl::~PDFCreatorImpl() {

}

uint32_t PDFCreatorImpl::CreateCustomEncryptedFile(
    const std::string& input_file_path,
    const std::string& filter_name,
    const std::vector<unsigned char> &publishing_license,
    std::shared_ptr<PDFCryptoHandler> crypto_hander,
    rmscrypto::api::SharedStream outputIOS) {
  uint32_t result = PDFCreatorErr::SUCCESS;
  file_path_ = input_file_path;

  CPDF_Parser pdf_parser;
  result = ParsePDFFile(&pdf_parser);
  if (PDFCreatorErr::SUCCESS != result) {
    return result;
  }

  CPDF_Dictionary* encryption_dictionary = CreateEncryptionDict(filter_name, publishing_license);
  result = CreatePDFFile(&pdf_parser, encryption_dictionary, crypto_hander, outputIOS);

  if (encryption_dictionary) {
    encryption_dictionary->Release();
    encryption_dictionary = NULL;
  }

  return result;
}

bool PDFCreatorImpl::IsProtectedByPassword(CPDF_Parser *pdf_parser) {
  CPDF_Dictionary* encryption_dictionary = pdf_parser->GetTrailer()->GetDict("Encrypt");
  if (encryption_dictionary) {
    CFX_ByteString filter_bytestring = encryption_dictionary->GetString("Filter");
    if (filter_bytestring.Compare("Standard") == 0) {
      return true;
    }
  }
  return false;
}

bool PDFCreatorImpl::IsSigned(CPDF_Parser *pdf_parser) {
  CPDF_Document* pdf_document = pdf_parser->GetDocument();
  CPDF_Dictionary* root_dictionary = pdf_document->GetRoot();
  if (root_dictionary) {
    if (CPDF_Dictionary* acro_form_dictionary = root_dictionary->GetDict("AcroForm")) {
      //has been signed?
      if (acro_form_dictionary->KeyExist("SigFlags")) {
        int signature_flag = acro_form_dictionary->GetInteger("SigFlags");
        if (signature_flag == 3) {
          return true;
        }
      }
    }
  }
  return false;
}

bool PDFCreatorImpl::IsDynamicXFA(CPDF_Parser *pdf_parser) {
  CPDF_Document* pdf_document = pdf_parser->GetDocument();
  CPDF_Dictionary* root_dictionary = pdf_document->GetRoot();
  CPDF_Dictionary* acro_form_dictionary = nullptr;
  if (!root_dictionary) return false;

  acro_form_dictionary = root_dictionary->GetDict("AcroForm");
  if (!acro_form_dictionary) return false;

  if (!acro_form_dictionary->KeyExist("XFA")) {
    return false;
  }

  CPDF_Object* pdf_xfa_object = acro_form_dictionary->GetElement("XFA");
  CPDF_Array* pdf_xfa_array = pdf_xfa_object->GetArray();
  if (!pdf_xfa_array) return false;

  FX_DWORD count = pdf_xfa_array->GetCount();
  for (int i = 0; i < static_cast<int>(count); i++) {
    CPDF_Object* pdf_object = pdf_xfa_array->GetElement(i);
    if (NULL != pdf_object && pdf_object->GetType() == PDFOBJ_REFERENCE) {
      CPDF_Object* pdf_reference_object = (CPDF_Object*)pdf_object->GetDict();
      if (NULL != pdf_reference_object) {
        CPDF_Dictionary* pdf_dictionary = pdf_reference_object->GetDict();
        if (NULL != pdf_dictionary && pdf_dictionary->GetInteger("Length") != 0) {
          if (root_dictionary->KeyExist("NeedsRendering")) {
            bool dynamic_xfa = (root_dictionary->GetBoolean("NeedsRendering") != 0);
            if (dynamic_xfa) {
              return true;
            } else {
              //is static XFA
              return false;
            }
          } else {
            //is static XFA
            return false;
          }
        }
      }
    }
  }
  return false;
}

uint32_t PDFCreatorImpl::ParsePDFFile(CPDF_Parser *pdf_parser) {
  uint32_t result = PDFCreatorErr::SUCCESS;
  CFX_ByteString file_path_bytestring = file_path_.c_str();

  FX_DWORD parse_result = pdf_parser->StartParse(file_path_bytestring);
  result = ConvertParsingErrCode(parse_result);

  if(result == PDFCreatorErr::SUCCESS) {
    //has been protected by password
    if (IsProtectedByPassword(pdf_parser)) {
      return PDFCreatorErr::SECURITY;
    }

    //has been signed?
    if (IsSigned(pdf_parser)) {
      return PDFCreatorErr::FORMAT;
    }

    //is dynamic XFA PDF
    if (IsDynamicXFA(pdf_parser)) {
      return PDFCreatorErr::FORMAT;
    }
  }

  return result;
}

uint32_t PDFCreatorImpl::ConvertParsingErrCode(FX_DWORD parse_result) {
  uint32_t result = PDFCreatorErr::SUCCESS;
  switch (parse_result) {
    case PDFPARSE_ERROR_SUCCESS: {
      result = PDFCreatorErr::SUCCESS;
      break;
    }
    case PDFPARSE_ERROR_FILE: {
      result = PDFCreatorErr::FILE;
      break;
    }
    case PDFPARSE_ERROR_FORMAT: {
      result = PDFCreatorErr::FORMAT;
      break;
    }

    case PDFPARSE_ERROR_PASSWORD:
    case PDFPARSE_ERROR_HANDLER:
    case PDFPARSE_ERROR_CERT: {
      result = PDFCreatorErr::SECURITY;
      break;
    }

    default: {
      result = PDFCreatorErr::UNKNOWN_ERR;
      break;
    }

  }

  return result;
}

CPDF_Dictionary* PDFCreatorImpl::CreateEncryptionDict(
    const std::string& filter_name,
    const std::vector<unsigned char> &publishing_license) {
  CPDF_Dictionary* encryption_dictionary = new CPDF_Dictionary;

  encryption_dictionary->SetAtName("Filter", filter_name.c_str());

  CPDF_Dictionary* pCFDic = new CPDF_Dictionary;

  CPDF_Dictionary* crypt_filter_dictionary = new CPDF_Dictionary;
  crypt_filter_dictionary->SetAtName("Type", "CryptFilter");
  crypt_filter_dictionary->SetAtName("CFM", "None");

  pCFDic->SetAt(filter_name.c_str(), crypt_filter_dictionary);

  encryption_dictionary->SetAt("CF", pCFDic);

  encryption_dictionary->SetAtName("StmF", filter_name.c_str());
  encryption_dictionary->SetAtName("StrF", filter_name.c_str());
  //encryption_dictionary->SetAtName("EFF", filter_name.c_str());
  encryption_dictionary->SetAtInteger("MicrosoftIRMVersion", 2);
  encryption_dictionary->SetAtInteger("V", 4);
  encryption_dictionary->SetAtBoolean("EncryptMetadata", true);

  const char* publishing_license_buffer =
      reinterpret_cast<const char*>(&(*publishing_license.begin()));
  uint32_t publishing_license_size = publishing_license.size();
  CFX_ByteString publishing_license_bytestring(publishing_license_buffer,
                                               publishing_license_size);
  encryption_dictionary->SetAtString("PublishingLicense", publishing_license_bytestring);
  return encryption_dictionary;
}

uint32_t PDFCreatorImpl::CreatePDFFile(
    CPDF_Parser *pdf_parser,
    CPDF_Dictionary* encryption_dictionary,
    std::shared_ptr<PDFCryptoHandler> crypto_hander,
    rmscrypto::api::SharedStream outputIOS) {
  uint32_t result = PDFCreatorErr::SUCCESS;

  CPDF_Document* pdf_document = pdf_parser->GetDocument();
  CPDF_Creator pdf_creator(pdf_document);

  CustomCryptoHandler cryptoHandler(crypto_hander);

  if (encryption_dictionary && crypto_hander) {
    FX_DWORD object_number = pdf_document->AddIndirectObject(encryption_dictionary);
    CPDF_Dictionary* trailer_dictionary = pdf_parser->GetTrailer();
    trailer_dictionary->SetAtReference("Encrypt", pdf_document, object_number);

    pdf_creator.SetCustomSecurity(encryption_dictionary, &cryptoHandler, true);

    CFX_ByteString file_path_bytestring = file_path_.c_str();
    CFX_WideString file_path_widestring = file_path_bytestring.UTF8Decode();
    CustomProgressiveEncryptHandler* progress_handler = new CustomProgressiveEncryptHandler(file_path_widestring);
    //pdf_creator takes over progress_handler
    pdf_creator.SetProgressiveEncryptHandler(progress_handler);
  }

  FileStreamImpl file_stream(outputIOS);
  FX_BOOL create_result = pdf_creator.Create(&file_stream);
  if (!create_result) {
    result = PDFCreatorErr::CREATOR;
  }

  return result;
}

uint32_t PDFCreatorImpl::UnprotectCustomEncryptedFile(
    rmscrypto::api::SharedStream inputIOS,
    const std::string& filter_name,
    std::shared_ptr<PDFSecurityHandler> security_hander,
    rmscrypto::api::SharedStream outputIOS) {
  uint32_t result = PDFCreatorErr::SUCCESS;

  PDFModuleMgrImpl::RegisterSecurityHandler(filter_name.c_str(), security_hander);

  FileStreamImpl input_file_stream(inputIOS);
  CPDF_Parser pdf_parser;
  FX_DWORD parse_result = pdf_parser.StartParse(&input_file_stream);
  result = ConvertParsingErrCode(parse_result);
  if (result != PDFCreatorErr::SUCCESS) return result;

  CPDF_Document* pdf_document = pdf_parser.GetDocument();
  CPDF_Creator pdf_creator(pdf_document);

  CPDF_Dictionary* encryption_dictionary = pdf_parser.GetEncryptDict();
  CPDF_Dictionary* trailer_dictionary = pdf_parser.GetTrailer();
  if (trailer_dictionary) {
    trailer_dictionary->RemoveAt("Encrypt");
  }
  if (encryption_dictionary) {
    pdf_document->DeleteIndirectObject(encryption_dictionary->GetObjNum());
  }

  pdf_creator.RemoveSecurity();
  pdf_creator.SetCustomSecurity(nullptr, nullptr, false);

  FileStreamImpl output_file_stream(outputIOS);
  FX_BOOL create_result = pdf_creator.Create(&output_file_stream);
  if (!create_result) {
    result = PDFCreatorErr::CREATOR;
  }

  return result;
}

} // namespace pdfobjectmodel
} // namespace rmscore
