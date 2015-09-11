/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <CryptoAPI.h>
#include <RMSCryptoExceptions.h>
#include "RestClientCache.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Filesystem/IFileSystem.h"
#include "../Platform/Settings/ILocalSettings.h"
#include "../Common/tools.h"
#include "../Platform/Logger/Logger.h"
#include <sstream>
#include <fstream>
#include <future>
#include <QStandardPaths>

using namespace std;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace restclients {
static const string END_USER_LICENSES_TAG   = "END_USER_LICENSES_UR";
static const string PUBLISHING_LICENSES_TAG = "PUBLISHING_LICENSES_UR";
static const string TEMPLATES_TAG           = "TEMPLATES_UR";
static const string CLOUD_DIAGNOSTICS_TAG   = "CLOUD_DIAGNOSTICS_UR";
static const string PERFORMANCE_TAG         = "PERFORMANCE_UR";
static const string DNS_CLIENT_RESULT_TAG   = "DNS_CLIENT_RESULT";

using SELF = RestClientCache;

// from IRestClientCache

RestClientCache::RestClientCache(CacheType type)
  : m_type(type) {}

common::StringArray RestClientCache::Lookup(
  const string& cacheName,
  const string& tag,
  const uint8_t *pbKey, size_t cbKey, bool useHash)
{
  Logger::Info("+RestClientCache::Lookup: cacheName=\"%s\", tag=\"%s\"",
               cacheName.data(),
               !tag.empty() ? tag.data() : "NULL");

  if (SELF::IsCacheLookupDisableTestHookOn())
  {
    return common::StringArray();
  }

  lock_guard<mutex> locker(SELF::cacheMutex);
  try
  {
    auto fileNamePattern = SELF::GetFileName(cacheName,
                                             tag,
                                             pbKey,
                                             cbKey,
                                             string(),
                                             true,
                                             useHash);

    auto fileSystem = platform::filesystem::IFileSystem::Create();

    auto vFileNames = fileSystem->QueryLocalStorageFiles(SELF::cacheFolderName,
                                                         fileNamePattern);

    common::StringArray vResponses;

    for (auto iFileName = vFileNames.begin(); vFileNames.end() != iFileName;
         ++iFileName)
    {
      try
      {
        if (SELF::DeleteIfExpired(cacheName, *iFileName))
        {
          continue;
        }

        auto filePath = SELF::cacheFolderName + (*iFileName);

        // open file stream
        auto ifs = std::make_shared<ifstream>(filePath, ios_base::in);

        // convert to input IStream
        auto iis = rmscrypto::api::CreateStreamFromStdStream(
          std::static_pointer_cast<std::istream>(ifs));

        if (iis) {
          // create input protected stream
          auto ips = m_type ==
                     CACHE_ENCRYPTED ? rmscrypto::api::
                     CreateCryptoStreamWithAutoKey(
            rmscrypto::api::CIPHER_MODE_CBC4K, filePath, iis) : iis;

          if (ips) {
            auto data = ips->Read(ips->Size());
            std::string dataStr(data.begin(), data.end());

            vResponses.push_back(dataStr);
          }
        }
      }
      catch (exceptions::RMSException)
      {
        Logger::Warning(
          "RestClientCache::Lookup: exception while reading a cached file \"%s\"",
          (*iFileName).data());

        //                LogException(LOG_MESSAGE_WARNING);

        // not fatal
        continue;
      }
      catch (rmscrypto::exceptions::RMSCryptoException& e)
      {
        Logger::Warning(
          "RestClientCache::Lookup: exception while work with crypto: \"%s\"",
          e.what());

        // fatal
        return vResponses;
      }
    }

    Logger::Info(
      "-RestClientCache::Lookup: cacheName=\"%s\", tag=\"%s\" returning %d result(s)",
      cacheName.data(),
      !tag.empty() ? tag.data() : "NULL",
      vResponses.size());


    return vResponses;
  }
  catch (exceptions::RMSException)
  {
    Logger::Warning(
      "RestClientCache::Lookup: exception while enumerating cached files.");

    //        LogException(LOG_MESSAGE_WARNING);

    // return 0 responses
    return common::StringArray();
  }
}

void RestClientCache::Store(
  const string& cacheName,
  const string& tag,
  const uint8_t *pbKey, size_t cbKey,
  const string& expires,
  const common::ByteArray& strResponse,
  bool useHash)
{
  Logger::Info(
    "RestClientCache::Store: cacheName=\"%s\", tag=\"%s\", expires=\"%s\"",
    cacheName.data(),
    !tag.empty() ? tag.data() : "NULL",
    !expires.empty() ? expires.data() : "NULL");

  lock_guard<mutex> locker(SELF::cacheMutex);
  try
  {
    auto fileName = SELF::GetFileName(cacheName,
                                      tag,
                                      pbKey,
                                      cbKey,
                                      expires,
                                      false,
                                      useHash);

    auto filePath = SELF::cacheFolderName + fileName;

    // create chache folder
    platform::filesystem::IFileSystem::CreateDirectory(SELF::cacheFolderName);

    // 4a63455a-cfa1-4ac6-bd2e-0d046cf1c3f7
    // john.smith@msopentechtest01.onmicrosoft.com
    // https://client.test.app
    // api.aadrm.com
    // open file stream
    auto ofs =
      std::make_shared<fstream>(filePath, ios_base::out | ios_base::trunc);

    // convert to output IStream
    auto ois =
      rmscrypto::api::CreateStreamFromStdStream(
        std::static_pointer_cast<std::ostream>(ofs));

    if (ois) {
      // create output protected stream
      auto ops = m_type ==
                 CACHE_ENCRYPTED ? rmscrypto::api::CreateCryptoStreamWithAutoKey(
        rmscrypto::api::CIPHER_MODE_CBC4K, filePath, ois) : ois;

      if (ops) {
        ops->Write(strResponse.data(), strResponse.size());
        ops->Flush();
      }
    }

    SELF::CleanupIfNeeded(cacheName);
  }
  catch (exceptions::RMSException)
  {
    Logger::Warning(
      "RestClientCache::Store: exception while caching the response.");
  }
  catch (rmscrypto::exceptions::RMSCryptoException& e)
  {
    Logger::Warning(
      "RestClientCache::Store: exception while work with crypto: \"%s\"",
      e.what());

    // fatal
    return;
  }
}

_ptr<ServiceDiscoveryDetails>RestClientCache::LookupServiceDiscoveryDetails(
  const string& domain)
{
  auto endUserLicensesUrl = this->Lookup(domain,
                                         END_USER_LICENSES_TAG,
                                         nullptr,
                                         0,
                                         false);
  auto publishingLicensesUrl = this->Lookup(domain,
                                            PUBLISHING_LICENSES_TAG,
                                            nullptr,
                                            0,
                                            false);
  auto templatesUrl = this->Lookup(domain,
                                   TEMPLATES_TAG,
                                   nullptr,
                                   0,
                                   false);
  auto cloudDiagnosticsServerUrl = this->Lookup(domain,
                                                CLOUD_DIAGNOSTICS_TAG,
                                                nullptr,
                                                0,
                                                false);
  auto performanceServerUrl = this->Lookup(domain,
                                           PERFORMANCE_TAG,
                                           nullptr,
                                           0,
                                           false);

  if (!endUserLicensesUrl.empty()
      && !publishingLicensesUrl.empty()
      && !templatesUrl.empty()
      && !cloudDiagnosticsServerUrl.empty()
      && !performanceServerUrl.empty())
  {
    auto details = make_shared<ServiceDiscoveryDetails>();
    details->EndUserLicensesUrl        = endUserLicensesUrl[0];
    details->PublishingLicensesUrl     = publishingLicensesUrl[0];
    details->TemplatesUrl              = templatesUrl[0];
    details->CloudDiagnosticsServerUrl = cloudDiagnosticsServerUrl[0];
    details->PerformanceServerUrl      = performanceServerUrl[0];

    return details;
  }

  return nullptr;
}

void RestClientCache::Store(
  const string               & domain,
  _ptr<ServiceDiscoveryDetails>serviceDiscoveryDetails,
  const string               & expires)
{
  this->Store(domain,
              END_USER_LICENSES_TAG,
              nullptr,
              0,
              expires,
              common::ByteArray(serviceDiscoveryDetails->EndUserLicensesUrl.begin(),
                                serviceDiscoveryDetails->EndUserLicensesUrl.end()),
              false);
  this->Store(domain,
              PUBLISHING_LICENSES_TAG,
              nullptr,
              0,
              expires,
              common::ByteArray(serviceDiscoveryDetails->PublishingLicensesUrl.
                                begin(),
                                serviceDiscoveryDetails->PublishingLicensesUrl.end()),
              false);
  this->Store(domain,
              TEMPLATES_TAG,
              nullptr,
              0,
              expires,
              common::ByteArray(serviceDiscoveryDetails->TemplatesUrl.begin(),
                                serviceDiscoveryDetails->TemplatesUrl.end()),
              false);
  this->Store(domain,
              CLOUD_DIAGNOSTICS_TAG,
              nullptr,
              0,
              expires,
              common::ByteArray(serviceDiscoveryDetails->CloudDiagnosticsServerUrl
                                .begin(),
                                serviceDiscoveryDetails->CloudDiagnosticsServerUrl
                                .end()),
              false);
  this->Store(domain,
              PERFORMANCE_TAG,
              nullptr,
              0,
              expires,
              common::ByteArray(serviceDiscoveryDetails->PerformanceServerUrl.
                                begin(),
                                serviceDiscoveryDetails->PerformanceServerUrl.end()),
              false);
}

common::StringArray RestClientCache::LookupDnsClientResult(
  const string& domain)
{
  return this->Lookup(domain, DNS_CLIENT_RESULT_TAG, nullptr, 0, false);
}

void RestClientCache::StoreDnsClientResult(
  const string& domain,
  const string& ttl,
  const string& dnsClientResult)
{
  this->Store(domain,
              DNS_CLIENT_RESULT_TAG,
              nullptr,
              0,
              ttl,
              common::ByteArray(dnsClientResult.begin(), dnsClientResult.end()),
              false);
}

// static /////////////////////////////////////////////////////////////

// the folder name, where we store the cache
const string RestClientCache::cacheFolderName = (QStandardPaths::writableLocation(
                                                   QStandardPaths::HomeLocation) +
                                                 "/.ms-ad/").toStdString();

// cache settings name constants
const string RestClientCache::cacheSettingsContainerName =
  "MSOPENTECHThin";
const string RestClientCache::cacheSettingsCacheLookupDisableTestHook =
  "TESTHOOK_IsCacheLookupDisabled";
const string RestClientCache::cacheCleanupFrequencySettingName =
  "CacheCleanupFrequency";
const string RestClientCache::cacheCleanupCounterSettingName =
  "CacheCleanupCounter";
const string RestClientCache::cacheMaximumFilesSettingName =
  "CacheMaximumFiles";

mutex RestClientCache::cacheMutex;

bool RestClientCache::IsCacheLookupDisableTestHookOn()
{
  bool res = platform::settings::ILocalSettings::Create()->GetBool(
    SELF::cacheSettingsContainerName,
    SELF::cacheSettingsCacheLookupDisableTestHook,
    false);

  Logger::Info("RestClientCache::IsCacheLookupDisableTestHookOn: %s state",
               res ? "TRUE" : "FALSE");

  return res;
}

// hashes the key and returns base64 of the hash
common::ByteArray RestClientCache::HashKey(const uint8_t *pbKey, size_t cbKey)
{
  auto cryptoEngine = rmscrypto::api::CreateCryptoEngine();
  auto sha256       = cryptoEngine->CreateHash(
    rmscrypto::api::CryptoHashAlgorithm::CRYPTO_HASH_ALGORITHM_SHA256);

  common::ByteArray vbHash(sha256->GetOutputSize());

  uint32_t cbHashSize = static_cast<uint32_t>(vbHash.size());

  // TODO.shch: remove (uint32_t&) cast
  sha256->Hash(pbKey, static_cast<uint32_t>(cbKey), &vbHash[0], cbHashSize);
  vbHash.resize(cbHashSize);

  common::ByteArray strBase64(common::ConvertBytesToBase64(vbHash));

  SELF::ReplaceNotAllowedCharactersInBase64(strBase64);

  return std::move(strBase64);
}

// gets the filename from the cache name, tag and key
string RestClientCache::GetFileName(
  const string& cacheName,
  const string& tag,
  const uint8_t *pbKey, size_t cbKey,
  const string& expires, bool pattern, bool useHash)
{
  //    const string fileNameFormat = "%s-%s=%s-%s.dat";
  bool bTagPattern = false;

  string tagMod(tag);

  if (tag.empty())
  {
    if (pattern)
    {
      // to enumerate all files
      tagMod      = "*?";
      bTagPattern = true;
    }
    else
    {
      // just a null tag
      tagMod = "NULL";
    }
  }

  string expiresMod(expires);

  if (expires.empty())
  {
    if (pattern)
    {
      // to enumerate all files
      expiresMod = "*?";
    }
    else
    {
      // just a null expiry time
      expiresMod = "0000-00-00T00:00Z";
    }
  }

  string nh("NoHash");
  string enAll("*?");
  common::ByteArray hash(nh.begin(), nh.end());

  if (useHash)
  {
    if (nullptr == pbKey)
    {
      //            CHECK_BOOL_X_HR(pattern, E_INVALIDARG);

      // enumerate all
      hash = common::ByteArray(enAll.begin(), enAll.end());
    }
    else
    {
      // get the hash of the key
      hash = HashKey(pbKey, cbKey);
    }
  }

  SELF::ReplaceNotAllowedCharactersInDateTime(expiresMod);

  if (!bTagPattern)
  {
    SELF::ReplaceNotAllowedCharactersInEmail(tagMod);
  }

  stringstream ss;
  string hashStr(hash.begin(), hash.end());
  ss << cacheName << "-" << expiresMod << "=" << tagMod << "-" << std::hex <<
    hashStr;

  return ss.str();
}

// parses the expiry time from the filename
common::DateTime RestClientCache::GetExpiryTimeFromFileName(
  const string& cacheName, const string& fileName)
{
  size_t nFileNameLength  = fileName.length();
  size_t nCacheNameLength = cacheName.length();

  if (nFileNameLength <= nCacheNameLength + 1)
  {
    throw exceptions::RMSInvalidArgumentException("Bad fileName");
  }

  size_t nExpiresOffset = nCacheNameLength + 1;

  auto expiresStart = nExpiresOffset;
  auto expiresEnd   = fileName.find('=');

  if (string::npos == expiresEnd)
  {
    throw exceptions::RMSInvalidArgumentException("Bad fileName");
  }

  auto strExpires = fileName.substr(nExpiresOffset, expiresEnd - expiresStart);

  SELF::ReplaceBackNotAllowedCharactersInDateTime(strExpires);

  return common::DateTime::fromString(strExpires.c_str(), Qt::ISODate);
}

// delete the file
void RestClientCache::DeleteCacheFile(const string& fileName)
{
  auto filePath = SELF::cacheFolderName + fileName;

  try
  {
    auto fileSystem = platform::filesystem::IFileSystem::Create();
    fileSystem->DeleteLocalStorageFile(filePath);
  }
  catch (exceptions::RMSException)
  {
    Logger::Warning(
      "RestClientCache::DeleteCacheFile: exception while deleting a file \"%s\".",
      fileName.data());

    //        LogException(LOG_MESSAGE_WARNING);
    // not fatal
  }
}

// delete if the file is expired
bool RestClientCache::DeleteIfExpired(
  const string& cacheName, const string& fileName)
{
  auto expires = SELF::GetExpiryTimeFromFileName(cacheName, fileName);

  if (expires.isNull()) return false;

  common::DateTime now = common::DateTime::currentDateTime();

  if (now > expires)
  {
    SELF::DeleteCacheFile(fileName);
    return true;
  }
  else
  {
    return false;
  }
}

// cleanup procedure
void RestClientCache::LaunchCleanup(const string& cacheName)
{
  // do cleanup in a separate thread
  auto result = async([cacheName]()
      {
        // need to lock as we don't want to delete the file while consuming it.
        lock_guard<mutex>locker(SELF::cacheMutex);
        try
        {
          Logger::Info("RestClientCache::LaunchCleanup: cleanup started.");


          auto pFileSystem = platform::filesystem::IFileSystem::Create();

          // get the pattern of all files in the cache
          auto fileNamePattern =
            SELF::GetFileName(cacheName,
                              string(),
                              nullptr,
                              0,
                              string(),
                              true,
                              true);

          // get the maximum number of files allowed in cache
          auto nMaximumFiles = SELF::GetCacheMaximumFiles(cacheName);

          // enumerate all files of the cache and returns the filenames ordered
          // by modified date in descending order
          auto allFileNames =
            pFileSystem->QueryLocalStorageFiles(cacheName, fileNamePattern);

          // go through the most recent nMaximumFiles or all the files
          for (std::size_t iFileName = 0;
               (iFileName < nMaximumFiles) && (iFileName < allFileNames.size());
               ++iFileName)
          {
            try
            {
              if (SELF::DeleteIfExpired(cacheName, allFileNames[iFileName]))
              {
                Logger::Info(
                  "RestClientCache::LaunchCleanup: deleted expired cache file \"%s\".",
                  allFileNames[iFileName].c_str());

// as we've deleted an expired file, there is one more slot
// left, so we need to increment maximum files number
                ++nMaximumFiles;
              }
            }
            catch (exceptions::RMSException)
            {
              //
              //
              //
              Logger::Warning(
                "RestClientCache::DeleteCacheFile: exception while checking if the file was expired \"%s\".",
                allFileNames[iFileName].c_str());

              //                    LogException(LOG_MESSAGE_WARNING);

              // not fatal, should continue with the next file
            }
          }

          // delete all the older ones
          for (auto iFileName = nMaximumFiles; iFileName < allFileNames.size();
               ++iFileName)
          {
            try
            {
              SELF::DeleteCacheFile(allFileNames[iFileName].c_str());

              //
              Logger::Info(
                "RestClientCache::LaunchCleanup: deleted old cache file \"%s\".",
                allFileNames[iFileName].c_str());
            }
            catch (exceptions::RMSException)
            {
              //
              //
              //
              Logger::Warning(
                "RestClientCache::DeleteCacheFile: exception while deleting a file \"%s\".",
                allFileNames[iFileName].c_str());

              //                    LogException(LOG_MESSAGE_WARNING);

              // not fatal, should continue with the next file
            }
          }
        }
        catch (exceptions::RMSException)
        {
          Logger::Warning(
            "RestClientCache::DeleteCacheFile: exception while cache cleanup.");
        }
        Logger::Info("RestClientCache::LaunchCleanup: cleanup finished.");
      });

  result.get();
}

// cleanup if needed
void RestClientCache::CleanupIfNeeded(const string& cacheName)
{
  try
  {
    // Note: We are ignoring the race conditions on accessing the counter. It is
    // ok if 2 threads
    // launch the cleanup procedure. It will affect the performance a little but
    // nothing more.

    int nCleanupCounter = SELF::GetCacheCleanupCounter(cacheName);

    if (nCleanupCounter <= 0)
    {
      // reset the counter before starting cleanup
      SELF::SetCacheCleanupCounter(cacheName,
                                   SELF::GetCacheCleanupFrequency(cacheName));

      Logger::Info("RestClientCache::CleanupIfNeeded: cleanup needed.");

      // now start the cleanup
      SELF::LaunchCleanup(cacheName);
    }
    else
    {
      // decrement the counter
      SELF::SetCacheCleanupCounter(cacheName, nCleanupCounter - 1);
    }
  }
  catch (exceptions::RMSException)
  {
    Logger::Warning(
      "RestClientCache::DeleteCacheFile: exception while checking if cleanup is needed.");

    //        LogException(LOG_MESSAGE_WARNING);
  }
}

// gets the cache cleanup frequency
int RestClientCache::GetCacheCleanupFrequency(const string& cacheName)
{
  auto settingName = GetCacheSettingName(cacheName,
                                         cacheCleanupFrequencySettingName);

  return platform::settings::ILocalSettings::Create()->GetInt(
    SELF::cacheSettingsContainerName,
    settingName,
    SELF::defaultCleanupFrequency);
}

// gets the cache cleanup counter
int RestClientCache::GetCacheCleanupCounter(const string& cacheName)
{
  auto settingName =
    GetCacheSettingName(cacheName, cacheCleanupCounterSettingName);

  return platform::settings::ILocalSettings::Create()->GetInt(
    SELF::cacheSettingsContainerName,
    settingName,
    SELF::defaultCleanupFrequency);
}

// sets the cache cleanup counter
void RestClientCache::SetCacheCleanupCounter(const string& cacheName,
                                             int           nCounter)
{
  auto settingName =
    GetCacheSettingName(cacheName, cacheCleanupCounterSettingName);

  return platform::settings::ILocalSettings::Create()->SetInt(
    SELF::cacheSettingsContainerName,
    settingName,
    nCounter);
}

// gets the cache maximum files number
size_t RestClientCache::GetCacheMaximumFiles(const string& cacheName)
{
  auto settingName = GetCacheSettingName(cacheName, cacheMaximumFilesSettingName);

  return platform::settings::ILocalSettings::Create()->GetInt(
    SELF::cacheSettingsContainerName,
    settingName,
    SELF::defaultMaximumFiles);
}

// gets the cache setting name from the cache name
string RestClientCache::GetCacheSettingName(
  const string& cacheName, const string& setting)
{
  return cacheName + "_" + setting;
}

// replace '/' or '+' character in the given base64 by '-' such that we can
// use it as a file name
void RestClientCache::ReplaceNotAllowedCharactersInBase64(
  common::ByteArray& strBase64)
{
  for (size_t iChar = 0; iChar < strBase64.size(); ++iChar)
  {
    if (('/' == strBase64[iChar]) || ('+' == strBase64[iChar]))
    {
      strBase64[iChar] = '-';
    }
  }
}

// replace ':' character in the given datetime by '_' such that we can use it
// as a file name
void RestClientCache::ReplaceNotAllowedCharactersInDateTime(string& strDate)
{
  for (size_t iChar = 0; iChar < strDate.size(); ++iChar)
  {
    if (':' == strDate[iChar])
    {
      strDate[iChar] = '_';
    }
  }
}

// replace '_' character in the given datetime by ':' such that we can parse
// it
void RestClientCache::ReplaceBackNotAllowedCharactersInDateTime(
  string& strDate)
{
  for (size_t iChar = 0; iChar < strDate.size(); ++iChar)
  {
    if ('_' == strDate[iChar])
    {
      strDate[iChar] = ':';
    }
  }
}

// replace special characters that are not allowed in filename
void RestClientCache::ReplaceNotAllowedCharactersInEmail(string& strEmail)
{
  for (size_t iChar = 0; iChar < strEmail.size(); ++iChar)
  {
    switch (strEmail[iChar])
    {
    case '|':
      strEmail[iChar] = ',';
      break;

    case '*':
      strEmail[iChar] = ';';
      break;

    case '?':
      strEmail[iChar] = '[';
      break;

    case '/':
      strEmail[iChar] = ']';
      break;
    }
  }
}

shared_ptr<IRestClientCache>IRestClientCache::Create(CacheType type)
{
  return make_shared<RestClientCache>(type);
}
}
} // namespace rmscore { namespace restclients {
