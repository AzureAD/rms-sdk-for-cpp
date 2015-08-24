/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _CRYPTO_STREAMS_LIB_IRMSENVIRONMENT_H
#define _CRYPTO_STREAMS_LIB_IRMSENVIRONMENT_H

#include <memory>

#include "CryptoAPIExport.h"

namespace rmscrypto {
namespace api {
class IRMSCryptoEnvironment {
public:

  virtual ~IRMSCryptoEnvironment() {}

  enum class LoggerOption : int { Always, Never };
  virtual void         LogOption(LoggerOption opt) = 0;
  virtual LoggerOption LogOption()                 = 0;
};

DLL_PUBLIC_CRYPTO std::shared_ptr<IRMSCryptoEnvironment>RMSCryptoEnvironment();
} // namespace api
} // namespace rmscrypto

#endif // _CRYPTO_STREAMS_LIB_IRMSENVIRONMENT_H
