/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_RESTCLIENTCACHE_H_
#define _RMS_LIB_RESTCLIENTCACHE_H_

#include "IRestClientCache.h"
#include <mutex>

namespace rmscore {
namespace restclients {
class RestClientCache : public IRestClientCache {
public:
  RestClientCache(CacheType type);

  common::StringArray Lookup(
    const std::string& cacheName,
    const std::string& tag,
    const uint8_t     *pbKey,
    size_t             cbKey,
    bool               useHash) override;

  void Store(const std::string      & cacheName,
             const std::string      & tag,
             const uint8_t           *pbKey,
             size_t                   cbKey,
             const std::string      & expires,
             const common::ByteArray& strResponse,
             bool                     useHash) override;

  _ptr<ServiceDiscoveryDetails>LookupServiceDiscoveryDetails(
    const std::string& domain) override;

  void                         Store(
    const std::string          & domain,
    _ptr<ServiceDiscoveryDetails>serviceDiscoveryDetails,
    const std::string          & expires) override;

  common::StringArray LookupDnsClientResult(
    const std::string& domain) override;

  void                StoreDnsClientResult(
    const std::string& domain,
    const std::string& ttl,
    const std::string& dnsClientResult) override;

  // returns if the cache is enabled
  static bool IsCacheLookupDisableTestHookOn();

private:
  CacheType m_type;

  // As the cache is per app, we don't use a global (i.e. named mutex). An app
  // should run in a single process.
  // Therefore a process-wise mutex should be ok.
  static std::mutex cacheMutex;

  // the folder name, where we store the cache
  static const std::string cacheFolderName;

  // hashes the key and returns base64 of the hash
  static common::ByteArray HashKey(const uint8_t *pbKey,
                                   size_t         cbKey);

  // gets the filename from the cache name, tag and key
  static std::string GetFileName(
    const std::string& cacheName,
    const std::string& tag,
    const uint8_t     *pbKey,
    size_t             cbKey,
    const std::string& expires,
    bool               pattern,
    bool               useHash);

  // parses the expiry time from the filename
  static common::DateTime GetExpiryTimeFromFileName(
    const std::string& cacheName,
    const std::string& fileName);

  // delete the file
  static void DeleteCacheFile(const std::string& fileName);

  // delete if the file is expired
  static bool DeleteIfExpired(
    const std::string& cacheName,
    const std::string& fileName);

  // cleanup procedure
  static void LaunchCleanup(const std::string& cacheName);

  // cleanup if needed
  static void CleanupIfNeeded(const std::string& cacheName);

  // cache settings name constants
  static const std::string cacheSettingsContainerName;
  static const std::string cacheSettingsCacheLookupDisableTestHook;
  static const std::string cacheCleanupFrequencySettingName;
  static const std::string cacheCleanupCounterSettingName;
  static const std::string cacheMaximumFilesSettingName;
  static const int defaultCleanupFrequency = 100;
  static const int defaultMaximumFiles     = 1000;

  // gets the cache cleanup frequency
  static int  GetCacheCleanupFrequency(const std::string& cacheName);

  // gets the cache cleanup counter
  static int  GetCacheCleanupCounter(const std::string& cacheName);

  // sets the cache cleanup counter
  static void SetCacheCleanupCounter(const std::string& cacheName,
                                     int                nCounter);

  // gets the cache maximum files number
  static size_t      GetCacheMaximumFiles(const std::string& cacheName);

  // gets the cache setting name from the cache name
  static std::string GetCacheSettingName(
    const std::string& cacheName,
    const std::string& setting);

  // replace '/' or '+' character in the given base64 by '-' such that we can
  // use it as a file name
  static void ReplaceNotAllowedCharactersInBase64(common::ByteArray& strBase64);

  // replace ':' character in the given datetime by '_' such that we can use it
  // as a file name
  static void ReplaceNotAllowedCharactersInDateTime(std::string& strDate);

  // replace '_' character in the given datetime by ':' such that we can parse
  // it
  static void ReplaceBackNotAllowedCharactersInDateTime(std::string& strDate);

  // replace special characters that are not allowed in filename
  static void ReplaceNotAllowedCharactersInEmail(std::string& strEmail);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_RESTCLIENTCACHE_H_
