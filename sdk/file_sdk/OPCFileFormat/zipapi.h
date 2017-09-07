#ifndef ZIPAPI_H
#define ZIPAPI_H

#include <Exceptions.h>

class ZipApi
{
public:
  ZipApi();
  std::string GetEntry(std::string filePath, std::string entryPath);
  int SetEntry(std::string filePath, std::string entryPath, std::string data);
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
