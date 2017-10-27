#include "UnencryptedWrapper.h"
#include "Basic.h"

namespace rmscore {
namespace pdfobjectmodel {

//////////////////////////////////////////////////////////////////////////
// class PDFWrapperDoc

std::unique_ptr<PDFWrapperDoc> PDFWrapperDoc::Create(rmscrypto::api::SharedStream input_stream)
{
    std::unique_ptr<PDFWrapperDoc> pdf_wrapper_doc(new PDFWrapperDocImpl(input_stream));
    return pdf_wrapper_doc;
}

PDFWrapperDocImpl::PDFWrapperDocImpl(rmscrypto::api::SharedStream wrapper_doc_stream)
{
    wrapper_doc_ = nullptr;
    wrapper_type_ = PDFWRAPPERDOC_TYPE_NORMAL;
    graphic_filter_ = L"";
    version_number_ = 0;
    payload_size_ = 0;
    file_name_ = L"";

    wrapper_file_stream_ = std::make_shared<FileStreamImpl>(wrapper_doc_stream);
    FX_DWORD parse_result = pdf_parser_.StartParse(wrapper_file_stream_.get());
    if(PDFPARSE_ERROR_SUCCESS == parse_result)
    {
        CPDF_Document* pdf_document = pdf_parser_.GetDocument();
        wrapper_doc_ = std::make_shared<CPDF_WrapperDoc>(pdf_document);

        wrapper_type_ = wrapper_doc_->GetWrapperType();
        if(wrapper_type_ == PDFWRAPPERDOC_TYPE_IRMV1)
        {
            CPDF_Dictionary* trailer_dictionary = pdf_parser_.GetTrailer();
            CPDF_Dictionary* wrapper_dictionary = trailer_dictionary->GetDict(IRMV1_WRAPPER_DICTIONARY);
            if(wrapper_dictionary)
            {
                CFX_ByteString bype_bytestring = wrapper_dictionary->GetString(IRMV1_WRAPPER_DICTIONARY_TYPE);
                CFX_WideString type_widestring = bype_bytestring.UTF8Decode();
                graphic_filter_ = (wchar_t*)(FX_LPCWSTR)type_widestring;
                version_number_ = 1;
                payload_size_ = trailer_dictionary->GetInteger(IRMV1_WRAPPER_DICTIONARY_OFFSET);
            }
        }
        else if(wrapper_type_ == PDFWRAPPERDOC_TYPE_IRMV2)
        {
            CFX_WideString graphic_filter_get;
            wrapper_doc_->GetCryptographicFilter(graphic_filter_get, version_number_);
            graphic_filter_ = (wchar_t*)(FX_LPCWSTR)graphic_filter_get;

            payload_size_ = wrapper_doc_->GetPayLoadSize();

            CFX_WideString file_name;
            wrapper_doc_->GetPayloadFileName(file_name);
            file_name_ = (wchar_t*)(FX_LPCWSTR)file_name;
        }
    }
}

PDFWrapperDocImpl::~PDFWrapperDocImpl()
{
    pdf_parser_.CloseParser();

    wrapper_file_stream_.reset();
    wrapper_file_stream_ = nullptr;

    wrapper_doc_.reset();
    wrapper_doc_ = nullptr;
}

uint32_t PDFWrapperDocImpl::GetWrapperType() const
{
    return wrapper_type_;
}

bool PDFWrapperDocImpl::GetCryptographicFilter(std::wstring& graphic_filter,
                                               float &version_num) const
{
    graphic_filter = graphic_filter_;
    version_num = version_number_;
    return true;
}

uint32_t PDFWrapperDocImpl::GetPayLoadSize() const
{
    return payload_size_;
}

bool PDFWrapperDocImpl::GetPayloadFileName(std::wstring& file_name) const
{
    file_name = file_name_;
    return true;
}

bool PDFWrapperDocImpl::StartGetPayload(rmscrypto::api::SharedStream output_stream)
{
    if(wrapper_type_ == PDFWRAPPERDOC_TYPE_IRMV1)
    {
        uint8_t* buffer_pointer_temp = new uint8_t[payload_size_];
        wrapper_file_stream_->ReadBlock(buffer_pointer_temp, 0, payload_size_);
        output_stream->Write(buffer_pointer_temp, payload_size_);
    }
    else if(wrapper_type_ == PDFWRAPPERDOC_TYPE_IRMV2)
    {
        if(!wrapper_doc_) return false;
        FileStreamImpl output_file_stream(output_stream);
        wrapper_doc_->StartGetPayload(&output_file_stream);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
// class PDFUnencryptedWrapperCreatorImpl
std::unique_ptr<PDFUnencryptedWrapperCreator> PDFUnencryptedWrapperCreator::Create(
        rmscrypto::api::SharedStream wrapper_doc_stream)
{
    std::unique_ptr<PDFUnencryptedWrapperCreator> pdfUnencryptedWrapperCreator(
                new PDFUnencryptedWrapperCreatorImpl(wrapper_doc_stream));
    return pdfUnencryptedWrapperCreator;
}

PDFUnencryptedWrapperCreatorImpl::PDFUnencryptedWrapperCreatorImpl(rmscrypto::api::SharedStream wrapper_doc_stream)
{
    pdf_wrapper_creator_ = nullptr;
    payload_file_stream_ = nullptr;

    wrapper_file_stream_ = std::make_shared<FileStreamImpl>(wrapper_doc_stream);
    FX_DWORD parse_result = pdf_parser_.StartParse(wrapper_file_stream_.get());
    if(PDFPARSE_ERROR_SUCCESS == parse_result)
    {
        CPDF_Document* pdf_document = pdf_parser_.GetDocument();
        pdf_wrapper_creator_ = FPDF_UnencryptedWrapperCreator_Create(pdf_document);
    }
}

PDFUnencryptedWrapperCreatorImpl::~PDFUnencryptedWrapperCreatorImpl()
{
     pdf_parser_.CloseParser();

    wrapper_file_stream_.reset();
    wrapper_file_stream_ = nullptr;

    if(payload_file_stream_)
    {
        payload_file_stream_.reset();
        payload_file_stream_ = nullptr;
    }

    if(pdf_wrapper_creator_)
    {
        pdf_wrapper_creator_->Release();
        pdf_wrapper_creator_ = nullptr;
    }
}

void PDFUnencryptedWrapperCreatorImpl::SetPayloadInfo(
        const std::wstring& sub_type,
        const std::wstring& file_name,
        const std::wstring& description,
        float version_num)
{
    if(!pdf_wrapper_creator_) return;
    CFX_WideString subtype_widestring = sub_type.c_str();
    CFX_WideString file_name_widestring = file_name.c_str();
    CFX_WideString description_widestring = description.c_str();
    pdf_wrapper_creator_->SetPayloadInfo(subtype_widestring,
                                         file_name_widestring,
                                         description_widestring,
                                         version_num);
}

void PDFUnencryptedWrapperCreatorImpl::SetPayLoad(rmscrypto::api::SharedStream input_stream)
{
    if(!pdf_wrapper_creator_) return;

    if(payload_file_stream_)
    {
        payload_file_stream_.reset();
        payload_file_stream_ = nullptr;
    }
    payload_file_stream_ = std::make_shared<FileStreamImpl>(input_stream);
    pdf_wrapper_creator_->SetPayLoad(payload_file_stream_.get());
}

bool PDFUnencryptedWrapperCreatorImpl::CreateUnencryptedWrapper(rmscrypto::api::SharedStream output_stream)
{
    if(!pdf_wrapper_creator_) return false;
    FileStreamImpl output_file_stream(output_stream);
    pdf_wrapper_creator_->Create(&output_file_stream);
    return true;
}

} // namespace pdfobjectmodel
} // namespace rmscore
