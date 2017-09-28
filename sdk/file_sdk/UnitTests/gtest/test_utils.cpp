#include "test_utils.h"

#include <memory>
#include <fstream>
#include <gtest/gtest.h>
#include <Common/std_stream_adapter.h>
#include <experimental/filesystem>

#include "executable_path.h"

namespace mip {
namespace file {

int g_argc = 0;
char** g_argv = nullptr;

void InitUnitTests(int argc, char** argv) {
  g_argc = argc;
  g_argv = argv;
}

std::string GetResourceFile(const std::string& fileName) {
  std::experimental::filesystem::path exe(GetExecutablePath(g_argv && g_argc > 0 ? g_argv[0] : nullptr));
  return exe.remove_filename()
    .append("Resources")
    .append(fileName)
    .string();
}

std::shared_ptr<IStream> GetIStreamFromFile(const std::string& fileName) {
  std::string fullPath = GetResourceFile(fileName);
  auto ifs = make_shared<std::ifstream>(fullPath, std::ios_base::binary);
  return StdStreamAdapter::Create(ifs);
}

} // namespace file
} // namespace mip
