#include "default_format.h"

namespace mip {
namespace file {

DefaultFormat::DefaultFormat(shared_ptr<IStream> file, const std::string& extension)
  : FileFormat(file, extension) {
}

const vector<pair<std::string, std::string> > DefaultFormat::GetProperties() {
  throw std::runtime_error("not implemented");
}

void DefaultFormat::Commit(shared_ptr<IStream> outputStream, string& newExtension) {
  throw std::runtime_error("not implemented");
}

void DefaultFormat::UpdateProperties(const std::vector<std::pair<std::string, std::string>>& propertiesToAdd, const std::vector<std::string>& keysToRemove) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
