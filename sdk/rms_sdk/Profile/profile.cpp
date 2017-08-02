/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "profile.h"

#include "logger.h"

namespace rmscore {

std::future<std::unique_ptr<Profile>> Profile::LoadAsync(const Settings& settings) {
  return std::async(settings.launchMode,
    [] (const Settings& settings) -> std::unique_ptr<Profile> {
    Logger::Init();
    LOG(INFO) << "Loading Profile for app " << settings.appIdentifier;
    return std::unique_ptr<Profile>(new Profile(settings));
  }, settings);
}

} // namespace rmscore
