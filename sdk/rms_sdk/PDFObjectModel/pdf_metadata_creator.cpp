#include "pdf_metadata_creator.h"

#include <cstdio>

#include "pdf_module_mgr.h"
#include "basic.h"

namespace rmscore {
namespace pdfobjectmodel {

//////////////////////////////////////////////////////////////////////////
//class PDFMetadataCreatorImpl

std::unique_ptr<PDFMetadataCreator> PDFMetadataCreator::Create(
    PDFSharedStream input_pdf_document_stream) {
  std::unique_ptr<PDFMetadataCreator> pdf_metadata_creator(
      new PDFMetadataCreatorImpl(input_pdf_document_stream));
  return pdf_metadata_creator;
}

PDFMetadataCreatorImpl::PDFMetadataCreatorImpl(PDFSharedStream input_pdf_document_stream) {
  pdf_document_stream_ = std::make_shared<FileStreamImpl>(input_pdf_document_stream);
  parse_result_ = pdf_parser_.StartParse(pdf_document_stream_.get());
  if (PDFPARSE_ERROR_SUCCESS == parse_result_) {
    CPDF_Document* pdf_document = pdf_parser_.GetDocument();
    pdf_metadata_.LoadDoc(pdf_document);
  }
}

PDFMetadataCreatorImpl::~PDFMetadataCreatorImpl() {

}

std::string PDFMetadataCreatorImpl::GetPDFVersion() {
  if (PDFPARSE_ERROR_SUCCESS != parse_result_) {
    return "";
  }

  FX_FLOAT version_number = pdf_parser_.GetFileVersion() / 10.0f;
  CFX_ByteString pdf_version;
  pdf_version.Format("PDF-%.1f", version_number);

  return (const char*)(FX_LPCSTR)pdf_version;
}

void PDFMetadataCreatorImpl::GetEntries(std::map<std::string, std::string>& entries) {
  if (PDFPARSE_ERROR_SUCCESS != parse_result_) {
    return;
  }

  CFX_WideStringArray keys;
  FX_INT32 key_count = pdf_metadata_.GetAllCustomKeys(keys);
  for (int i=0; i<key_count; i++) {
    FX_INT32 use_info_or_xml = CPDF_METADATATYPE_INFO;
    CFX_WideString value;
    pdf_metadata_.GetString(keys[i], value, use_info_or_xml);

    std::string key_utf8 = keys[i].UTF8Encode();
    std::string value_utf8 = value.UTF8Encode();

    entries.insert(std::pair<std::string, std::string>(key_utf8, value_utf8));
  }
}

void PDFMetadataCreatorImpl::RemoveEntry(std::string key) {
  if (PDFPARSE_ERROR_SUCCESS != parse_result_) {
    return;
  }

  CFX_ByteString key_utf8 = key.c_str();
  CFX_WideString key_unicode = key_utf8.UTF8Decode();
  pdf_metadata_.DeleteCustomKey(key_unicode);
  pdf_metadata_.SyncUpdate();
}

void PDFMetadataCreatorImpl::AddEntry(const std::string& key, const std::string& value) {
  if (PDFPARSE_ERROR_SUCCESS != parse_result_) {
    return;
  }

  CFX_ByteString key_utf8 = key.c_str();
  CFX_WideString key_unicode = key_utf8.UTF8Decode();

  CFX_ByteString value_utf8 = value.c_str();
  CFX_WideString value_unicode = value_utf8.UTF8Decode();
  pdf_metadata_.SetString(key_unicode, value_unicode);
  pdf_metadata_.SyncUpdate();
}

bool PDFMetadataCreatorImpl::Save(PDFSharedStream output_pdf_document_stream) {
  if (PDFPARSE_ERROR_SUCCESS != parse_result_) {
    return false;
  }

  CPDF_Document* pdf_document = pdf_parser_.GetDocument();
  CPDF_Creator pdf_creator(pdf_document);

  FileStreamImpl output_file_stream(output_pdf_document_stream);
  FX_BOOL create_result = pdf_creator.Create(&output_file_stream);
  if (!create_result) {
    return false;
  }
  return true;
}

} // namespace pdfobjectmodel
} // namespace rmscore
