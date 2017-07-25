#include "profile.h"
#include "Telemetry.h

namespace rmscore {

std::shared_future<std::shared_ptr<Profile>> Profile::LoadAsync(const Settings& settings) {
  return std::async(settings.GetLaunchMode(),
    [] (const Settings& settings) -> std::shared_ptr<Profile> {
    Telemetry::Initialize(tenantToken);
    return std::make_shared<Profile>(settings);
  }, settings);
}

  /* A typical Aria workflow:
   * LogManager::Initialize();
   * auto pLogger = LogManager::GetLogger();
   * EventProperties logEventProperty("Dunkirk");
   * logEventProperty.SetProperty("Rating", "5star");
   * pLogger->LogEvent(logEventProperty);
   * LogManager::FlushAndTeardown();
   */

std::shared_future<RmsStatus> Profile::UnloadAsync() {
  return std::async(settings.GetLaunchMode(),
    [] () -> RmsStatus {
      try {
        Telemetry::FlushAndTeardown();
      }
      catch (...) {
        return Error!;
      }
      return Success!;
    });
}

} // namespace rmscore
