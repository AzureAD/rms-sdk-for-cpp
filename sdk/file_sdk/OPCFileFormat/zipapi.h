#ifndef ZIPAPI_H
#define ZIPAPI_H

#include <Exceptions.h>
#include "gsfinputistream.h"
#include <map>
#include <string>
#include <memory>

class ZipApi
{
public:
  ZipApi();
  std::string GetEntry(std::shared_ptr<IStream> stream, std::string entryPath);
  int SetEntry(const std::string& filePath, const std::string& outputFilePath, std::map<std::string,std::string>& entries);

};

class ZipException : public rmsauth::Exception
{
public:
  ZipException(const std::string& error, const std::string& message) : Exception(error, message){}
  ZipException(const std::string& error) : ZipException(error, ""){}
};

class ZipEntryNotFoundException : public ZipException
{
public:
  ZipEntryNotFoundException(const std::string& error, const std::string& message) : ZipException(error, message){}
  ZipEntryNotFoundException(const std::string& error) : ZipEntryNotFoundException(error, ""){}
};

#endif // ZIPAPI_H
