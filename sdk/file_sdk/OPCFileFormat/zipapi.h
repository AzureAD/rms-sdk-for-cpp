#ifndef ZIPAPI_H
#define ZIPAPI_H

#include <string>

class ZipApi
{
public:
  ZipApi();
  std::string GetEntry(std::string filePath, std::string entryPath);
  int SetEntry(std::string filePath, std::string EntryPath, std::string data);
};

#endif // ZIPAPI_H
