#ifndef ZIPAPI_H
#define ZIPAPI_H

#include <iostream>
#include <string>

class ZipApi
{
public:
    std::string GetEntry(std::string filePath, std::string EntryPath);
    int SetEntry(std::string filePath, std::string EntryPath, std::string data);
};

#endif // ZIPAPI_H
