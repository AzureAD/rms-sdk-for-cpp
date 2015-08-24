/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _CRYPTO_STREAMS_LIB_IRMSENVIRONMENT_IMPL_H
#define _CRYPTO_STREAMS_LIB_IRMSENVIRONMENT_IMPL_H

#include <mutex>
#include <QAtomicInt>

#include "../../CryptoAPI/IRMSCryptoEnvironment.h"

namespace rmscrypto {
namespace platform {
namespace settings {
class IRMSCryptoEnvironmentImpl : public api::IRMSCryptoEnvironment {
public:

  IRMSCryptoEnvironmentImpl();
  virtual ~IRMSCryptoEnvironmentImpl() {}

  virtual void                                      LogOption(LoggerOption opt);
  virtual LoggerOption                              LogOption();

  static std::shared_ptr<api::IRMSCryptoEnvironment>Environment();

private:

  QAtomicInt _optLog;
};

extern std::shared_ptr<IRMSCryptoEnvironmentImpl> _instance;
} // namespace settings
} // namespace platform
} // namespace rmscrypto

#endif // _CRYPTO_STREAMS_LIB_IRMSENVIRONMENT_IMPL_H
