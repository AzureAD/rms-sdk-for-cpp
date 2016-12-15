/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "StorageAccessWindows.h"
#include <strsafe.h>
#include <CryptoAPI/RMSCryptoExceptions.h>
#include <Platform/Logger/Logger.h>

using namespace rmscrypto::exceptions;
using namespace rmscrypto::platform::logger;
using namespace std;

static const string TableName = "MSIPCKeyStorage";
static const string KeyWrapperColumn = "csKeyWrapper";
static const string KeyColumn = "csKey";

/**
 * Helper Functions
 */

string GetLastWin32ErrorAsString()
{
    unsigned long errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return string();

    char* messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&messageBuffer, 0, NULL);

    string message(messageBuffer, size);

    LocalFree(messageBuffer);

    return message;
}

/**
 * StorageAccessWindows Singleton implementation.
 * If the class has already been created, the static variable will not be created again.
 * Starting MSVC 2015 (which supports Magic Statics), this implementation is also thread-safe.
 */
StorageAccessWindows& StorageAccessWindows::Instance()
{
    static StorageAccessWindows mInstance;
    return mInstance;
}

StorageAccessWindows::StorageAccessWindows()
{
    const wstring directory = CreateLocalStorage();
    const wstring dbName = directory + L"\\DefaultMSIPCKeyStorage.db";

    sqlite3* mDbTemp;
    int rc = sqlite3_open16(dbName.c_str(), &mDbTemp);
    mDb.reset(mDbTemp);
    ErrorHandler(rc, "CreateTable");

    string sqlCreateTable = CreateTableQuery(TableName);
    rc = sqlite3_exec(mDb.get(), sqlCreateTable.c_str(), NULL, NULL, NULL);
    ErrorHandler(rc, "CreateTable");
}

wstring StorageAccessWindows::CreateLocalStorage()
{
    /**
     * Fetch environment variable %localappdata%.
     * Check if db folder exists.
     * If not, create and encrypt.
     */

    const wstring lpName (L"localappdata");
    const wstring aipDir (L"\\Microsoft\\RMSLocalStorage");
    unsigned long bufferSize = GetEnvironmentVariableW(lpName.c_str(), NULL, 0);
    if (bufferSize == 0)
    {
        throw RMSCryptoIOKeyException(GetLastWin32ErrorAsString().c_str());
    }

    wstring lpBuffer (bufferSize, L'\0');
    if (!GetEnvironmentVariableW(lpName.c_str(), &lpBuffer[0], lpBuffer.length()))
    {
        throw RMSCryptoIOKeyException(GetLastWin32ErrorAsString().c_str());
    }
    /**
     * Resizing the buffer to exclude the terminating null character.
     * Read https://msdn.microsoft.com/en-us/magazine/mt238407.aspx for
     * best practices of using STL strings at Win32 API boundaries.
     */
    lpBuffer.resize(bufferSize-1);

    wstring directory = lpBuffer + aipDir;
    if (!CreateDirectory(directory.c_str(), NULL))
    {
        unsigned long lastError = GetLastError();
        /**
         * Other than the first time this code runs, the directory creation will always return
         * ERROR_ALREADY_EXISTS and hence that is not a fatal error.
         */
        if (lastError != ERROR_ALREADY_EXISTS)
        {
            throw RMSCryptoIOKeyException(GetLastWin32ErrorAsString().c_str());
        }
    }
    else
    {
        if (!EncryptFile(directory.c_str()))
        {
            Logger::Warning("EncryptFile failed with " + GetLastWin32ErrorAsString());
        }
    }
    return directory;
}

void StorageAccessWindows::StoreKey(const string& csKeyWrapper,const string& csKey)
{
    string sqlInsert = StoreQuery(csKeyWrapper, csKey, TableName);
    int rc = sqlite3_exec(mDb.get(), sqlInsert.c_str(), NULL, NULL, NULL);
    ErrorHandler(rc, "Store");
}

shared_ptr<string> StorageAccessWindows::LookupKey(const string& csKeyWrapper)
{
    string sqlLookup = LookupQuery(csKeyWrapper, TableName);
    char** results = NULL;
    int rows, columns;
    int rc = sqlite3_get_table(mDb.get(), sqlLookup.c_str(), &results, &rows, &columns, NULL);
    ErrorHandler(rc, "Lookup");
    shared_ptr<string> result;
    if (rows >= 1 && columns >= 1)
    {
        result.reset(new string(results[1]));
        sqlite3_free_table(results);
    }
    return result;
}

void StorageAccessWindows::RemoveKey(const string& csKeyWrapper)
{
    string sqlDelete = RemoveQuery(csKeyWrapper, TableName);
    int rc = sqlite3_exec(mDb.get(), sqlDelete.c_str(), NULL, NULL, NULL);
    ErrorHandler(rc, "Remove");
}

string StorageAccessWindows::CreateTableQuery(const string& TableName)
{
    return "CREATE TABLE IF NOT EXISTS " + TableName + " (" + KeyWrapperColumn +
            " STRING PRIMARY KEY, " + KeyColumn + " STRING);";
}

string StorageAccessWindows::StoreQuery(
        const string& keyWrapper,
        const string& key,
        const string& TableName)
{
    return "INSERT OR REPLACE INTO " + TableName + " VALUES('"+ keyWrapper +"','" + key + "');";
}

string StorageAccessWindows::LookupQuery(const string& keyWrapper, const string& TableName)
{
    return "SELECT " + KeyColumn + " FROM " + TableName + " WHERE " + KeyWrapperColumn + "='" + keyWrapper + "';";
}

string StorageAccessWindows::RemoveQuery(const string& keyWrapper, const string& TableName)
{
    return "DELETE FROM " + TableName + " WHERE " + KeyWrapperColumn + "='" + keyWrapper + "';";
}

void StorageAccessWindows::ErrorHandler(int returnCode, const string& errorOp)
{
    if (returnCode)
    {
        Logger::Error("StorageAccessWindows failure in " + errorOp + ", sqlite3 failure " + sqlite3_errmsg(mDb.get()));
        throw RMSCryptoIOKeyException("StorageAccessWindows failure in operation " + errorOp);
    }
}
