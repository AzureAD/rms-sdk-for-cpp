#ifndef RMSSDK_PDFOBJECTMODEL_UNENCRYPTEDWRAPPER_H_
#define RMSSDK_PDFOBJECTMODEL_UNENCRYPTEDWRAPPER_H_

#include "pdf_object_model.h"
#include "core.inc"
#include "basic.h"

namespace rmscore {
namespace pdfobjectmodel {

#define IRMV1_WRAPPER_DICTIONARY            "Wrapper"
#define IRMV1_WRAPPER_DICTIONARY_TYPE       "Type"
#define IRMV1_WRAPPER_DICTIONARY_VERSION    "Version"
#define IRMV1_WRAPPER_DICTIONARY_OFFSET     "WrapperOffset"

/**
 * @brief The implementaion class of interface class PDFWrapperDoc defined in pdf object model layer.
 * The implementation is on the basic of Foxit core.
 * Please refer to comments of PDFWrapperDoc.
 */
class PDFWrapperDocImpl : public PDFWrapperDoc {
 public:
  explicit PDFWrapperDocImpl(PDFSharedStream wrapper_doc_stream);
  virtual ~PDFWrapperDocImpl();

  virtual uint32_t GetWrapperType() const;

  virtual bool GetCryptographicFilter(std::wstring& graphic_filter, float &version_num) const;

  virtual uint32_t GetPayLoadSize() const;

  //this is just for IRM V2.
  virtual bool GetPayloadFileName(std::wstring& file_name) const;

  virtual bool StartGetPayload(PDFSharedStream output_stream);

 private:
  std::shared_ptr<FileStreamImpl> wrapper_file_stream_;
  CPDF_Parser pdf_parser_;
  //this is just for IRM V2. we have to parse IRM V1 self.
  std::shared_ptr<CPDF_WrapperDoc> wrapper_doc_;

  uint32_t wrapper_type_;
  std::wstring graphic_filter_;
  float version_number_;
  uint32_t payload_size_;
  std::wstring file_name_;
};

/**
 * @brief The implementaion class of interface class PDFUnencryptedWrapperCreator defined in pdf object model layer.
 * The implementation is on the basic of Foxit core.
 * Please refer to comments of PDFUnencryptedWrapperCreator.
 */
class PDFUnencryptedWrapperCreatorImpl : public PDFUnencryptedWrapperCreator {
 public:
  explicit PDFUnencryptedWrapperCreatorImpl(PDFSharedStream wrapper_doc_stream);
  virtual ~PDFUnencryptedWrapperCreatorImpl();

  virtual void SetPayloadInfo(
      const std::wstring& sub_type,
      const std::wstring& file_name,
      const std::wstring& description,
      float version_num);

  virtual void SetPayLoad(PDFSharedStream input_stream);

  virtual bool CreateUnencryptedWrapper(PDFSharedStream output_stream);

 private:
  std::shared_ptr<FileStreamImpl> wrapper_file_stream_;
  CPDF_Parser pdf_parser_;
  IPDF_UnencryptedWrapperCreator* pdf_wrapper_creator_;
  std::shared_ptr<FileStreamImpl> payload_file_stream_;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMSSDK_PDFOBJECTMODEL_UNENCRYPTEDWRAPPER_H_
