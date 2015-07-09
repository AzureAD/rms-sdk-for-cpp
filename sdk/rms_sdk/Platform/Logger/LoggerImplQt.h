/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_LOGGERQTIMPL_H_
#define _RMS_LIB_LOGGERQTIMPL_H_

#include "Logger.h"
#include <fstream>

namespace rmscore {
namespace platform {
namespace logger {
class LoggerImplQt : public Logger {
public:

  ~LoggerImplQt();

protected:

  virtual void append(const std::string& prefix,
                      const std::string& record) override;

private:

  LoggerImplQt();
  friend class Logger;
  std::ofstream stream_;
};
} // namespace logger
} // namespace platform
} // namespace rmscore

#endif // _RMS_LIB_LOGGERQTIMPL_H_
