#include "pdf_file_format.h"

namespace mip {
namespace file {

PDFFileFormat::PDFFileFormat(std::shared_ptr<IStream> inputStream, const string& extension)
  : FileFormat(inputStream, extension)
{
}

const vector<pair<std::string, std::string> > PDFFileFormat::GetProperties() {
  throw std::runtime_error("not implemented");
}

void PDFFileFormat::Commit(std::shared_ptr<IStream> outputStream, string& newExtension) {
  throw std::runtime_error("not implemented");
}

void PDFFileFormat::UpdateProperties(const std::vector<std::pair<std::string, std::string>>& propertiesToAdd, const std::vector<std::string>& keysToRemove) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
