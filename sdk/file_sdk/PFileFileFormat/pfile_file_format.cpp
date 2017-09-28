#include "pfile_file_format.h"

namespace mip {
namespace file {

PFileFileFormat::PFileFileFormat(std::shared_ptr<IStream> inputStream, const string& extension)
  : FileFormat(inputStream, extension) {
}

string PFileFileFormat::GetOriginalExtension() {
  throw std::runtime_error("not implemented");
}

const vector<pair<std::string, std::string> > PFileFileFormat::GetProperties() {
  throw std::runtime_error("not implemented");
}

void PFileFileFormat::Commit(std::shared_ptr<IStream> outputStream, string& newExtension) {
  throw std::runtime_error("not implemented");
}

void PFileFileFormat::UpdateProperties(const std::vector<std::pair<std::string, std::string>>& propertiesToAdd, const std::vector<std::string>& keysToRemove) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
