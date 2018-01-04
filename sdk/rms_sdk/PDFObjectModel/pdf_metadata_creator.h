#ifndef RMSSDK_PDFOBJECTMODEL_PDFMETADATACREATOR_H_
#define RMSSDK_PDFOBJECTMODEL_PDFMETADATACREATOR_H_

#include "pdf_object_model.h"
#include "core.h"
#include "basic.h"

namespace rmscore {
namespace pdfobjectmodel {

/**
 * @brief The implementation class of interface class PDFMetadataCreator defined in pdf object model layer.
 * Please refer to comments of PDFMetadataCreator.
 */
class PDFMetadataCreatorImpl : public PDFMetadataCreator {
 public:
  explicit PDFMetadataCreatorImpl(PDFSharedStream input_pdf_document_stream);
  virtual ~PDFMetadataCreatorImpl();

  virtual std::string GetPDFVersion();

  virtual void GetEntries(std::map<std::string, std::string>& entries);

  virtual void RemoveEntry(std::string key);

  virtual void AddEntry(const std::string& key, const std::string& value);

  virtual bool Save(PDFSharedStream output_pdf_document_stream);

 protected:

 private:
  std::shared_ptr<FileStreamImpl> pdf_document_stream_;
  CPDF_Parser pdf_parser_;
  FX_DWORD parse_result_;
  CPDF_Metadata pdf_metadata_;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMSSDK_PDFOBJECTMODEL_PDFMETADATACREATOR_H_
