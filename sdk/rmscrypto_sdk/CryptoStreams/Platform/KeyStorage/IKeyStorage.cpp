#include "IKeyStorage.h"
#include <QtGlobal>

#if defined(Q_OS_UNIX) && !defined(Q_OS_OSX)
#include "KeyStoragePosix.h"
namespace rmscrypto { namespace platform { namespace keystorage {
std::shared_ptr<IKeyStorage>IKeyStorage::Create() {
  return std::shared_ptr<KeyStoragePosix>(new KeyStoragePosix);
}
}}}
#elif defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include "KeyStorageWindows.h"
namespace rmscrypto { namespace platform { namespace keystorage {
std::shared_ptr<IKeyStorage>IKeyStorage::Create() {
    throw std::logic_error("not implemented");
    return std::shared_ptr<KeyStorageWindows>(new KeyStorageWindows);
}
}}}
#elif defined(Q_OS_OSX)
#include "KeyStorageOSX.h"
namespace rmscrypto { namespace platform { namespace keystorage {
std::shared_ptr<IKeyStorage>IKeyStorage::Create() {
    throw std::logic_error("not implemented");
    return std::shared_ptr<KeyStorageOSX>(new KeyStorageOSX);
}
}}}
#else
namespace rmscrypto { namespace platform { namespace keystorage {
std::shared_ptr<IKeyStorage>IKeyStorage::Create() {
  throw std::logic_error("Unexpected OS");
}
}}}
#endif
