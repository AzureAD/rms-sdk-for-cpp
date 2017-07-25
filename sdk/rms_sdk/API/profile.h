#ifndef PROFILE_H
#define PROFILE_H

#include <future>
#include <memory>
#include "Protector.h"

namespace rmscore {

class Profile {
public:
  class Settings {
  public:
    Settings(
      const std::string& path,
      const std::string& appId,
      const std::string& userId,
      std::launch launchMode)
      : mPath(path),
      mAppId(appId),
      mUserId(userId),
      mLaunchMode(launchMode){
    }
    ~Settings() { }

    const std::string& GetPath() const { return mPath; }
    const std::launch GetLaunchMode() const { return mLaunchMode; }

  private:
    std::string mPath;
    std::string mAppId;
    std::string mUserId;
    std::launch mLaunchMode;
  };

  static std::shared_future<std::shared_ptr<Profile>> LoadAsync(const Settings& settings);
  static std::shared_future<RmsStatus> UnloadAsync()

  std::unique_ptr<Protector> Create(
      const std::string& fileName,
      std::shared_ptr<std::fstream> inputStream,
      std::string& outputFileName);

  const Settings& GetSettings() const {
    return mSettings;
  }

  Profile(const Settings& settings) : mSettings(settings) { }

private:
  Settings mSettings;
};

}

#endif // PROFILE_H
