#include "profile_test.h"

#include <QDebug>

#include "profile.h"
#include "string_utils.h"

using rmscore::Profile;
using namespace rmscore::stringutils;

void ProfileTest::TestProfileLoad() {
  Profile::Settings settings ("Some Path", "Some App", std::launch::deferred);
  std::string expectedPath = settings.path + GetDirectorySeparator() + settings.appIdentifier;
  auto fut = Profile::LoadAsync(settings);
  auto profile = fut.get();
  QVERIFY2(profile != nullptr, "nullptr returned for profile");
  QVERIFY(profile->GetProfilePath() == expectedPath);
}
