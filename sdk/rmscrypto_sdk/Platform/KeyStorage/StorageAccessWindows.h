/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef STORAGEACCESSWINDOWS_H
#define STORAGEACCESSWINDOWS_H
#include "sqlite3.h"
#include <atomic>
#include <string>
#include <memory>
#include <mutex>
#include <Windows.h>

class StorageAccessWindows {

public:
   static StorageAccessWindows& Instance();
   void StoreKey(const std::string& keyWrapper, const std::string& key);
   std::shared_ptr<std::string> LookupKey(const std::string& keyWrapper);
   void RemoveKey(const std::string& keyWrapper);

private:
   StorageAccessWindows();
   // Disallow copy and assignment
   StorageAccessWindows(StorageAccessWindows const&){}
   StorageAccessWindows& operator=(StorageAccessWindows const&){}
   std::wstring CreateLocalStorage();
   std::string CreateTableQuery(const std::string& TableName);
   std::string StoreQuery(const std::string& keyWrapper, const std::string& key, const std::string& TableName);
   std::string LookupQuery(const std::string& keyWrapper, const std::string& TableName);
   std::string RemoveQuery(const std::string& keyWrapper, const std::string& TableName);
   void ErrorHandler(int returnCode, const std::string& operation);

   std::shared_ptr<sqlite3> mDb;
};

#endif // STORAGEACCESSWINDOWS_H
