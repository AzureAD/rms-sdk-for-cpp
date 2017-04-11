/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <StaticLogger.h>
#include <FileCache.h>
#include <Constants.h>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

using namespace std;

namespace rmsauth {
Mutex FileCache::fileLock_;

FileCache::FileCache(const String& filePath)
{
  if (filePath.empty()) // using default settings
  {
    const QString fileName = "token_cache.dat";
    auto path              = QStandardPaths::writableLocation(
      QStandardPaths::HomeLocation) + "/.ms-ad";

    if (!QDir().mkpath(path))
    {
      throw RmsauthException("Can't create cache directory");
    }
    QFileInfo fi(QDir(path), fileName);
    cacheFilePath_ = fi.absoluteFilePath().toStdString();
  }
  else
  {
    QFileInfo fi(QString::fromStdString(filePath));
    auto userDefinedCacheDir = fi.path();

    if (!QDir().exists(userDefinedCacheDir))
    {
      throw RmsauthException("Can't find user defined cache directory",
                             userDefinedCacheDir.toStdString());
    }
    cacheFilePath_ = fi.absoluteFilePath().toStdString();
  }
  StaticLogger::Info(Tag(),"path: %", cacheFilePath_);
}

void FileCache::readCache()
{
  StaticLogger::Info(Tag(),"readCache");
  ifstream ifs(cacheFilePath_, ios::binary | ios::ate);

  if (!ifs.is_open())
  {
    StaticLogger::Info(Tag(),"Cache file doesn't exist! '%'", cacheFilePath_);
    return;
  }
  ifstream::pos_type pos = ifs.tellg();

  ByteArray cacheData(pos);

  ifs.seekg(0, ios::beg);
  ifs.read(&cacheData[0], pos);
  deserialize(cacheData);
  ifs.close();
}

void FileCache::writeCache()
{
  StaticLogger::Info(Tag(),"writeCache");
  ofstream ofs(cacheFilePath_, ios::binary | ios::trunc);

  if (!ofs.is_open())
  {
    StaticLogger::Info(Tag(),"Can't open cache file for writing! '%'", cacheFilePath_);
    return;
  }

  auto cacheData = serialize();
  ofs.write(&cacheData[0], cacheData.size());
  ofs.close();
}

void FileCache::clear()
{
  StaticLogger::Info(Tag(),"clear");
  TokenCache::clear();
  Lock l(fileLock_);
  bool ok = QFile::remove(cacheFilePath_.c_str());

  if (!ok)
  {
    StaticLogger::Error(Tag(),"clear: Failed to delete a file: ");
  }
}

void FileCache::onBeforeAccess(const TokenCacheNotificationArgs& /* args*/)
{
  StaticLogger::Info(Tag(),"onBeforeAccess");
  Lock l(fileLock_);

  readCache();
}

void FileCache::onAfterAccess(const TokenCacheNotificationArgs& /* args*/)
{
  StaticLogger::Info(Tag(),"onAfterAccess");
  Lock l(fileLock_);

  // if the access operation resulted in a cache update
  if (hasStateChanged_)
  {
    writeCache();
    hasStateChanged_ = false;
  }
}
} // namespace rmsauth {
