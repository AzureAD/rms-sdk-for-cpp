#include "test_utils.h"

#include <memory>
#include <fstream>
#include <gtest/gtest.h>
#include <Common/std_stream_adapter.h>

namespace mip {
namespace file {

std::string GetResourceFile(const std::string& fileName) {
  std::string path("Resources\\"); // TODO: use path relative to executable
  return path + fileName;
}

std::shared_ptr<IStream> GetIStreamFromFile(const std::string& fileName) {
  std::string fullPath = GetResourceFile(fileName);
  auto ifs = make_shared<std::ifstream>(fullPath, std::ios_base::binary);
  return StdStreamAdapter::Create(ifs);
}

} // namespace file
} // namespace mip
