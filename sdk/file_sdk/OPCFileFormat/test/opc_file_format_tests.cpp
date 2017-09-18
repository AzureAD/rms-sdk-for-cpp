#include "OPCFileFormat/opc_file_format.h"
#include "gtest/gtest.h"

namespace mip {
namespace file {

TEST(OPCFileFormatTests, ReadTags)
{
	OPCFileFormat f(shared_ptr<mip::file::IStream>(), "docx");
	f.GetTags();
}

} // namespace mip
} // namespace file
