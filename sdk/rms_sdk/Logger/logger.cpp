/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/
#include "logger.h"

#include <mutex>

INITIALIZE_EASYLOGGINGPP
/*
To log run the macro LOG(<LEVEL>) << "Example";
The existing levels are: Global, Trace, Debug, Fatal, Error, Warning, Info, Verbose, Unknown
*/

using std::string;

namespace rmscore {

std::once_flag flag;

static void InitHelper() {
  el::Configurations default_conf;
  default_conf.setToDefault();
  default_conf.set(el::Level::Global,
      el::ConfigurationType::Format, "%thread %level %datetime %msg %fbase:%line");
  default_conf.set(el::Level::Global,
      el::ConfigurationType::Filename, "logs/mip_sdk.log");
  default_conf.set(el::Level::Global,
      el::ConfigurationType::Enabled, "true");
  default_conf.set(el::Level::Global,
      el::ConfigurationType::ToFile, "true");
  default_conf.set(el::Level::Global,
      el::ConfigurationType::ToStandardOutput, "false");
  default_conf.set(el::Level::Global,
      el::ConfigurationType::SubsecondPrecision, "3");
  default_conf.set(el::Level::Global,
      el::ConfigurationType::PerformanceTracking, "true");
  default_conf.set(el::Level::Global,
      el::ConfigurationType::MaxLogFileSize, "2097152"); // 2 MB
  default_conf.set(el::Level::Global,
      el::ConfigurationType::LogFlushThreshold, "5");

  el::Loggers::reconfigureLogger("default", default_conf);
  el::Loggers::reconfigureAllLoggers(default_conf);
}

void Logger::Init() {
    std::call_once(flag, InitHelper);
}

} // namespace rmscore
