#include "unencrypted_wrapper.h"
#include "basic.h"

namespace rmscore {
namespace pdfobjectmodel {

//////////////////////////////////////////////////////////////////////////
// class PDFWrapperDoc

std::unique_ptr<PDFWrapperDoc> PDFWrapperDoc::Create(PDFSharedStream input_stream) {
  std::unique_ptr<PDFWrapperDoc> pdf_wrapper_doc(new PDFWrapperDocImpl(input_stream));
  return pdf_wrapper_doc;
}

PDFWrapperDocImpl::PDFWrapperDocImpl(PDFSharedStream wrapper_doc_stream) {
  wrapper_doc_ = nullptr;
  wrapper_type_ = PDFWrapperDocType::NORMAL;
  graphic_filter_ = L"";
  version_number_ = 0;
  payload_size_ = 0;
  file_name_ = L"";
  is_irmv2_without_wrapper = false;

  uint64_t stream_size = wrapper_doc_stream->GetSize();

  wrapper_file_stream_ = std::make_shared<FileStreamImpl>(wrapper_doc_stream);
  FX_DWORD parse_result = pdf_parser_.StartParse(wrapper_file_stream_.get());
  if (PDFPARSE_ERROR_SUCCESS == parse_result) {
    CPDF_Document* pdf_document = pdf_parser_.GetDocument();
    wrapper_doc_ = std::make_shared<CPDF_WrapperDoc>(pdf_document);

    wrapper_type_ = static_cast<PDFWrapperDocType>(wrapper_doc_->GetWrapperType());
    if (wrapper_type_ == PDFWrapperDocType::IRMV1) {
      CPDF_Dictionary* trailer_dictionary = pdf_parser_.GetTrailer();
      CPDF_Dictionary* wrapper_dictionary = trailer_dictionary->GetDict(IRMV1_WRAPPER_DICTIONARY);
      if (wrapper_dictionary) {
        CFX_ByteString bype_bytestring = wrapper_dictionary->GetString(IRMV1_WRAPPER_DICTIONARY_TYPE);
        CFX_WideString type_widestring = bype_bytestring.UTF8Decode();
        graphic_filter_ = (wchar_t*)(FX_LPCWSTR)type_widestring;
        version_number_ = 1;
        payload_size_ = trailer_dictionary->GetInteger(IRMV1_WRAPPER_DICTIONARY_OFFSET);
      }
    } else if (wrapper_type_ == PDFWrapperDocType::IRMV2) {
      CFX_WideString graphic_filter_get;
      wrapper_doc_->GetCryptographicFilter(graphic_filter_get, version_number_);
      graphic_filter_ = (wchar_t*)(FX_LPCWSTR)graphic_filter_get;

      payload_size_ = wrapper_doc_->GetPayLoadSize();

      CFX_WideString file_name;
      wrapper_doc_->GetPayloadFileName(file_name);
      file_name_ = (wchar_t*)(FX_LPCWSTR)file_name;
    } else {
      bool isPasswordProtected = IsProtectedByPassword(&pdf_parser_);
      if (isPasswordProtected) {
        wrapper_type_ = PDFWrapperDocType::NOT_IRM_ENCRYPTED;
      }
      bool isSigned = IsSigned(&pdf_parser_);
      if (isSigned) {
        wrapper_type_ = PDFWrapperDocType::DIGITALLY_SIGNED;
      }
    }
  } else if (PDFPARSE_ERROR_HANDLER == parse_result) {
      CPDF_Dictionary* encryption_dictionary = pdf_parser_.GetEncryptDict();
      CFX_ByteString filter_name = encryption_dictionary->GetString("Filter");
      if (filter_name.EqualNoCase(PDF_PROTECTOR_FILTER_NAME)) {
        FX_INT32 irm_version = encryption_dictionary->GetInteger("MicrosoftIRMVersion");
        if(irm_version <= 1) {
          wrapper_type_ = PDFWrapperDocType::IRMV1;
          version_number_ = 1;
          CFX_WideString filter_name_widestring = filter_name.UTF8Decode();
          graphic_filter_ = (wchar_t*)(FX_LPCWSTR)filter_name_widestring;
          payload_size_ = static_cast<uint32_t>(stream_size);
        } else {
          wrapper_type_ = PDFWrapperDocType::IRMV2;
          version_number_ = 2;
          CFX_WideString filter_name_widestring = filter_name.UTF8Decode();
          graphic_filter_ = (wchar_t*)(FX_LPCWSTR)filter_name_widestring;
          payload_size_ = static_cast<uint32_t>(stream_size);
          is_irmv2_without_wrapper = true;
        }
      } else {
        wrapper_type_ = PDFWrapperDocType::NOT_IRM_ENCRYPTED;
      }
  } else if (PDFPARSE_ERROR_PASSWORD == parse_result ||
             PDFPARSE_ERROR_CERT == parse_result) {
    wrapper_type_ = PDFWrapperDocType::NOT_IRM_ENCRYPTED;
  } else {
    wrapper_type_ = PDFWrapperDocType::NOT_COMPATIBLE_WITH_IRM;
  }
}

PDFWrapperDocImpl::~PDFWrapperDocImpl() {
  pdf_parser_.CloseParser();

  wrapper_file_stream_ = nullptr;
  wrapper_doc_ = nullptr;
}

PDFWrapperDocType PDFWrapperDocImpl::GetWrapperType() const {
  return wrapper_type_;
}

bool PDFWrapperDocImpl::GetCryptographicFilter(std::wstring& graphic_filter,
                                               float &version_num) const {
  graphic_filter = graphic_filter_;
  version_num = version_number_;
  return true;
}

uint32_t PDFWrapperDocImpl::GetPayLoadSize() const {
  return payload_size_;
}

bool PDFWrapperDocImpl::GetPayloadFileName(std::wstring& file_name) const {
  file_name = file_name_;
  return true;
}

bool PDFWrapperDocImpl::StartGetPayload(PDFSharedStream output_stream) {
  if (wrapper_type_ == PDFWrapperDocType::IRMV1 || is_irmv2_without_wrapper) {
    std::vector<uint8_t> vec_payload(payload_size_);
    wrapper_file_stream_->ReadBlock(reinterpret_cast<uint8_t *>(&vec_payload[0]), 0, payload_size_);
    output_stream->WriteBlock(reinterpret_cast<uint8_t *>(&vec_payload[0]), 0, static_cast<uint64_t>(payload_size_));
  } else if (wrapper_type_ == PDFWrapperDocType::IRMV2) {
    if (!wrapper_doc_) return false;
    FileStreamImpl output_file_stream(output_stream);
    wrapper_doc_->StartGetPayload(&output_file_stream);
  }

  return true;
}

bool PDFWrapperDocImpl::IsProtectedByPassword(CPDF_Parser *pdf_parser) {
  CPDF_Dictionary* encryption_dictionary = pdf_parser->GetTrailer()->GetDict("Encrypt");
  if (encryption_dictionary) {
    CFX_ByteString filter_bytestring = encryption_dictionary->GetString("Filter");
    if (filter_bytestring.Compare("Standard") == 0) {
      return true;
    }
  }
  return false;
}

bool PDFWrapperDocImpl::IsSigned(CPDF_Parser *pdf_parser) {
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

//////////////////////////////////////////////////////////////////////////
// class PDFUnencryptedWrapperCreatorImpl
std::unique_ptr<PDFUnencryptedWrapperCreator> PDFUnencryptedWrapperCreator::Create(
    PDFSharedStream wrapper_doc_stream) {
  std::unique_ptr<PDFUnencryptedWrapperCreator> pdfUnencryptedWrapperCreator(
      new PDFUnencryptedWrapperCreatorImpl(wrapper_doc_stream));
  return pdfUnencryptedWrapperCreator;
}

PDFUnencryptedWrapperCreatorImpl::PDFUnencryptedWrapperCreatorImpl(
    PDFSharedStream wrapper_doc_stream) {
  pdf_wrapper_creator_ = nullptr;
  payload_file_stream_ = nullptr;

  wrapper_file_stream_ = std::make_shared<FileStreamImpl>(wrapper_doc_stream);
  FX_DWORD parse_result = pdf_parser_.StartParse(wrapper_file_stream_.get());
  if (PDFPARSE_ERROR_SUCCESS == parse_result) {
    CPDF_Document* pdf_document = pdf_parser_.GetDocument();
    pdf_wrapper_creator_ = FPDF_UnencryptedWrapperCreator_Create(pdf_document);
  }
}

PDFUnencryptedWrapperCreatorImpl::~PDFUnencryptedWrapperCreatorImpl() {
  pdf_parser_.CloseParser();

  wrapper_file_stream_ = nullptr;

  if (payload_file_stream_) {
    payload_file_stream_ = nullptr;
  }

  if (pdf_wrapper_creator_) {
    pdf_wrapper_creator_->Release();
    pdf_wrapper_creator_ = nullptr;
  }
}

void PDFUnencryptedWrapperCreatorImpl::SetPayloadInfo(
    const std::wstring& sub_type,
    const std::wstring& file_name,
    const std::wstring& description,
    float version_num) {
  if (!pdf_wrapper_creator_) return;
  CFX_WideString subtype_widestring = sub_type.c_str();
  CFX_WideString file_name_widestring = file_name.c_str();
  CFX_WideString description_widestring = description.c_str();
  pdf_wrapper_creator_->SetPayloadInfo(
      subtype_widestring,
      file_name_widestring,
      description_widestring,
      version_num);
}

void PDFUnencryptedWrapperCreatorImpl::SetPayLoad(PDFSharedStream input_stream) {
  if (!pdf_wrapper_creator_) return;

  if (payload_file_stream_) {
    payload_file_stream_.reset();
    payload_file_stream_ = nullptr;
  }
  payload_file_stream_ = std::make_shared<FileStreamImpl>(input_stream);
  pdf_wrapper_creator_->SetPayLoad(payload_file_stream_.get());
}

bool PDFUnencryptedWrapperCreatorImpl::CreateUnencryptedWrapper(const std::string& cache_file_path, PDFSharedStream output_stream) {
  if (!pdf_wrapper_creator_) return false;
  FileStreamImpl output_file_stream(output_stream);

  CFX_ByteString file_path_bytestring = cache_file_path.c_str();
  CFX_WideString file_path_widestring = file_path_bytestring.UTF8Decode();
  CPDF_CreatorOptionImpl creator_option(file_path_widestring);
  if (!cache_file_path.empty()) {
    pdf_wrapper_creator_->SetCreatorOption(&creator_option);
  }

  FX_BOOL bCreate = pdf_wrapper_creator_->Create(&output_file_stream, FPDFCREATE_PROGRESSIVE);
  if (bCreate) {
    FX_INT32 continue_count = 0;
    do {
      continue_count = pdf_wrapper_creator_->Continue();
    } while (continue_count != 0);
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////
// class CPDF_CreatorOptionImpl
CPDF_CreatorOptionImpl::CPDF_CreatorOptionImpl(const CFX_WideString& temp_path) {
  temp_path_ = temp_path;
}

CPDF_CreatorOptionImpl::~CPDF_CreatorOptionImpl() {

}

IFX_FileStream* CPDF_CreatorOptionImpl::GetTempFile(CPDF_Object* pObj) {
  if (pObj->GetType() != PDFOBJ_STREAM) return nullptr;
  FX_INT32 raw_size = ((CPDF_Stream*)pObj)->GetRawSize();
  if (raw_size > MIN_WRAPPER_CACHE_SIZE) {
    CFX_WideString path = temp_path_;
    IFX_FileStream* file_stream = FX_CreateFileStream(path, FX_FILEMODE_Truncate);
    return file_stream;
  } else {
    return nullptr;
  }
}

void CPDF_CreatorOptionImpl::ReleaseTempFile(IFX_FileStream* file_stream) {
  if (file_stream) {
    file_stream->Release();

    CFX_WideString path = temp_path_;

    CFX_ByteString utf8_path = path.UTF8Encode();
    std::remove((const char *)(FX_LPCSTR)utf8_path);
  }
}

} // namespace pdfobjectmodel
} // namespace rmscore
