/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifdef QTFRAMEWORK
#include "LoggerImplQt.h"
#include <time.h>
#include <sstream>
#include <string>

namespace rmscrypto {
namespace platform {
namespace logger {
Logger& Logger::instance() {
  static LoggerImplQt instance;

  return instance;
}

static std::string localTime(const char *format) {
  const time_t rawtime = time(nullptr);

#ifdef Q_OS_WIN32
  tm timebuf;
  localtime_s(&timebuf, &rawtime);
#else // ifdef Q_OS_WIN32
  tm timebuf = *localtime(&rawtime);
#endif // ifdef Q_OS_WIN32
  const int   BUFF_LEN = 32;
  std::string res(BUFF_LEN, '-');
  auto len = strftime(&res[0], BUFF_LEN, format, &timebuf);
  res.resize(len);
  return res;
}

LoggerImplQt::LoggerImplQt() {
  std::stringstream filename;

  filename << "rmscrypto_log_" << localTime("%H%M%S-%d%m") << ".log";

  this->stream_.open(filename.str(), std::ofstream::out | std::ofstream::trunc);

  if (this->stream_.fail()) {
    Logger::Hidden("Can't open file: %s", filename.str().c_str());
  }
}

LoggerImplQt::~LoggerImplQt() {
  this->stream_.close();
}

void LoggerImplQt::append(const std::string& prefix, const std::string& record) {
  std::stringstream ss;

  ss << localTime("%H:%M:%S ") << prefix.c_str() << ": " << record;
  this->stream_ << ss.str() << std::endl;
}
} // namespace logger
} // namespace platform
} // namespace rmscrypto
#endif // QTFRAMEWORK
