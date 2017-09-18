#ifndef FILE_OPCFILEFORMAT_ZIPAPI_H
#define FILE_OPCFILEFORMAT_ZIPAPI_H

//#include <Exceptions.h>
#include <map>
#include <string>
#include <memory>
#include <Common/IStream.h>

namespace mip {
namespace file {
		
using mip::file::IStream;

class ZipFile
{
public:
	ZipFile(std::shared_ptr<IStream> inputStream)
	{
	}

	std::string GetEntry(const std::string& entryPath)
	{
		return {};
	}

	void SetEntry(const std::string& entryPath, const std::string& content)
	{
	}

	void Commit(std::shared_ptr<IStream> outputStream)
	{
	}
};
//
//class ZipException : public rmsauth::Exception
//{
//public:
//	ZipException(const std::string& error, const std::string& message) : Exception(error, message) {}
//	ZipException(const std::string& error) : ZipException(error, "") {}
//};
//
//class ZipEntryNotFoundException : public ZipException
//{
//public:
//	ZipEntryNotFoundException(const std::string& error, const std::string& message) : ZipException(error, message) {}
//	ZipEntryNotFoundException(const std::string& error) : ZipEntryNotFoundException(error, "") {}
//};

} // namespace file
} // namespace mip

#endif // FILE_OPCFILEFORMAT_ZIPAPI_H
