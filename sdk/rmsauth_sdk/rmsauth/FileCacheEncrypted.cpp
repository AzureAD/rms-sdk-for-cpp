/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <Logger.h>
#include <FileCacheEncrypted.h>
#include <Constants.h>
#include <fstream>
#include "../../rmscrypto_sdk/CryptoAPI/CryptoAPI.h"

using namespace std;

namespace rmsauth {

FileCacheEncrypted::FileCacheEncrypted(const String& filePath)
    : FileCache(filePath)
{
}

/// throws:
///     rmscrypto::exceptions::RMSCryptoException
void FileCacheEncrypted::readCache()
{
    Logger::info(Tag(), "readCache");
    ifstream ifs(cacheFilePath_, ios::binary|ios::ate);
    if (!ifs.is_open())
    {
        Logger::info(Tag(), "Cache file doesn't exist! '%'", cacheFilePath_);
        return;
    }

    ifstream::pos_type pos = ifs.tellg();
    if (pos <= 0)
    {
        Logger::info(Tag(), "File is empty! '%'", cacheFilePath_);
        return;
    }

    ByteArray cacheData(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&cacheData[0], pos);

    // decrypt cacheData;
    auto inputDataPtr = std::make_shared<std::vector<uint8_t>>(cacheData.begin(), cacheData.end());
    auto cacheDataDecryptedPtr = rmscrypto::api::DecryptWithAutoKey(inputDataPtr);
    ByteArray cacheDataDecrypted(cacheDataDecryptedPtr->begin(), cacheDataDecryptedPtr->end());

    deserialize(cacheDataDecrypted);
    ifs.close();
}

/// throws:
///     rmsauth::RmsauthParsingException
///     rmscrypto::exceptions::RMSCryptoException
void FileCacheEncrypted::writeCache()
{
    Logger::info(Tag(), "writeCache");
    ofstream ofs(cacheFilePath_, ios::binary|ios::app);
    if (!ofs.is_open())
    {
        Logger::info(Tag(), "Can't open cache file for writing! '%'", cacheFilePath_);
        return;
    }

    auto cacheData = serialize();

    // encrypt cacheData
    Logger::info(Tag(), "encrypting cacheData");
    auto inputDataPtr = std::make_shared<std::vector<uint8_t>>(cacheData.begin(), cacheData.end());
    auto cacheDataEncryptedPtr = rmscrypto::api::EncryptWithAutoKey(inputDataPtr);
    ByteArray cacheDataEncrypted(cacheDataEncryptedPtr->begin(), cacheDataEncryptedPtr->end());

    Logger::info(Tag(), "writing to the file stream");
    ofs.write(&cacheDataEncrypted[0], cacheDataEncrypted.size());

    if(-1 == ofs.tellp())
    {
        throw RmsauthParsingException("writeCache", "write data error");
    }
    ofs.close();
}

} // namespace rmsauth {
