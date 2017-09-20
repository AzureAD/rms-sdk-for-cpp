/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PROFILE_PROFILE_H
#define PROFILE_PROFILE_H

#include <future>
#include <memory>

#include "api_export.h"
#include "string_utils.h"

namespace rmscore {

class Profile {
public:
  DLL_PUBLIC_RMS
  struct Settings {
    Settings (
        const std::string& _path = ".ms-ad",
        const std::string& _appId = "aip_sdk",
        std::launch _launchMode = std::launch::async) :
        path(_path),
        appIdentifier(_appId),
        launchMode(_launchMode){ }

    std::string path;
    std::string appIdentifier;
    std::launch launchMode;
  };

  DLL_PUBLIC_RMS
  static std::future<std::unique_ptr<Profile>> LoadAsync(const Settings& settings);

  const std::string& GetProfilePath() const {
    return mProfilePath;
  }

  const Settings& GetSettings() const {
    return mSettings;
  }
  
private:
  Profile(const Settings& settings) : mSettings(settings) {
    mProfilePath = mSettings.path + stringutils::GetDirectorySeparator() + mSettings.appIdentifier;
  }

  Settings mSettings;
  std::string mProfilePath;
};

} // namespace rmscore

#endif // PROFILE_PROFILE_H
