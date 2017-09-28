#include "OPCFileFormat/opc_file_format.h"
#include "OPCFileFormat/zip_file.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Common/std_stream_adapter.h"
#include <fstream>
#include "UnitTests/gtest/test_utils.h"

namespace mip {
namespace file {

using namespace testing;

class MockIStream : public IStream {
public:
  MOCK_METHOD2(Read, int64_t(uint8_t* buffer, int64_t bufferLength));
  MOCK_METHOD2(Write, int64_t(const uint8_t* buffer, int64_t bufferLength));
  MOCK_METHOD0(Flush, bool());
  MOCK_METHOD1(Seek, void(uint64_t position));
  MOCK_CONST_METHOD0(CanRead, bool());
  MOCK_CONST_METHOD0(CanWrite, bool());
  MOCK_METHOD0(Position, uint64_t());
  MOCK_METHOD0(Size, uint64_t());
  MOCK_METHOD1(Size, void(uint64_t value));
  MOCK_METHOD0(Clone, shared_ptr<IStream>());
};

TEST(OPCFileFormatTests, EmptyFile_GetProperties_ThrowsZipException) {
  auto stream = make_shared<MockIStream>();
  auto& streamMock = *stream;
  EXPECT_CALL(streamMock, Size())
    .WillOnce(Return(0));

  OPCFileFormat f(stream, "docx");
  EXPECT_THROW(f.GetProperties(), ZipException);
}

TEST(OPCFileFormatTests, GetProperties) {
  shared_ptr<IStream> stream = GetIStreamFromFile("LabeledGeneral.docx");
  OPCFileFormat f(stream, "docx");
  vector<pair<string, string>> props = f.GetProperties();
}

} // namespace mip
} // namespace file
