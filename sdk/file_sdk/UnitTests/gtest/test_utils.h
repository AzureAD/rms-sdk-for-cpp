#ifndef FILE_SDK_TEST_UTILS_H
#define FILE_SDK_TEST_UTILS_H

#include <memory>
#include "Common/istream.h"

namespace mip {
namespace file {

void InitUnitTests(int argc, char** argv);

std::string GetResourceFile(const std::string& fileName);

std::shared_ptr<IStream> GetIStreamFromFile(const std::string& fileName);

} // namespace file
} // namespace mip

#endif // FILE_SDK_TEST_UTILS_H
