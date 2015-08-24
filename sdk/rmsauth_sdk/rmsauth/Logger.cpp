/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <Logger.h>
#include <Exceptions.h>

namespace rmsauth {

Logger& Logger::instance()
{
    static LoggerImpl instance;
    return instance;
}

void Logger::printf(StringStream& ss, const char *s)
{
    while (*s)
    {
        if (*s == '%')
        {
            if (*(s + 1) == '%')
            {
                ++s;
            }
            else
            {
                throw Exception("Logger: invalid format string: missing arguments");
            }
        }
        ss << *s++;
    }
}

void Logger::record(const String& category, const String& tag, const String& record)
{
    auto env = RMSAuthEnvironment();
    if (!env || (env->LogOption() == IRMSAuthEnvironment::LoggerOption::Never)) {
      return;
    }
    Logger::instance().append(category, tag, record);
}

void Logger::info(const String& tag, const String& record)
{
    Logger::record("INF", tag, record);
}
void Logger::warning(const String& tag, const String& record)
{
    Logger::record("WRN", tag, record);
}
void Logger::error(const String& tag, const String& record)
{
    Logger::record("ERR", tag, record);
}

LoggerImpl::LoggerImpl()
{
    StringStream filename;
    filename << "rmsauth_" << LoggerImpl::getLocalTime("HHmmss-MMdd") << ".log";

    this->stream_.open (filename.str(), std::ofstream::out | std::ofstream::trunc);
    if( this->stream_.fail())
    {
        StringStream info;
        info << "Can't open file: " << filename.str();
        throw std::runtime_error(info.str());
    }
}
LoggerImpl::~LoggerImpl()
{
    this->stream_.close();
}
void LoggerImpl::append(const String& category, const String& tag, const std::string& record)
{
    std::stringstream ss;
    ss << LoggerImpl::getLocalTime("HH:mm:ss ") << category << ": " << tag << ": " << record;
    this->stream_ << ss.str() << std::endl;
}

} // namespace rmsauth {
