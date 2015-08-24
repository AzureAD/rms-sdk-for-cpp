/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <fstream>
#include "types.h"
#include "rmsauthExport.h"
#include "IRMSAuthEnvironment.h"

namespace rmsauth {

class Logger
{
public:
    template<typename T, typename... Args>
    static void record(const String& category, const String& tag, const String& record, T value, Args... args);

    template<typename T, typename... Args>
    static void info(const String& tag, const String& record, T value, Args... args);

    template<typename T, typename... Args>
    static void warning(const String& tag, const String& record, T value, Args... args);


    template<typename T, typename... Args>
    static void error(const String& tag, const String& record, T value, Args... args);

    static void record(const String& category, const String& tag, const String& record);
    static void info(const String& tag, const String& record);
    /// to enable these records - set environment variable: export RMS_HIDDEN_LOG=ON
    static void hidden(const String& tag, const String& record);
    static void warning(const String& tag, const String& record);
    static void error(const String& tag, const String& record);
    virtual ~Logger(){}

protected:
    virtual void append(const String& category, const String& tag, const String& record) = 0;

private:
    static Logger& instance();

    static void printf(StringStream& ss, const char *s);

    template<typename T, typename... Args>
    static void printf(StringStream& ss, const char *s, T value, Args... args);
};

template<typename T, typename... Args>
void Logger::record(const String& category, const String& tag, const String& record, T value, Args... args)
{
    auto env = RMSAuthEnvironment();
    if (!env || (env->LogOption() == IRMSAuthEnvironment::LoggerOption::Never)) {
      return;
    }

    StringStream ss;
    Logger::printf(ss, record.c_str(), value, args...);
    Logger::instance().append(category, tag, ss.str());
}

template<typename T, typename... Args>
void Logger::info(const String& tag, const String& record, T value, Args... args)
{
    Logger::record("INF", tag, record, value, args...);
}

template<typename T, typename... Args>
void Logger::warning(const String& tag, const String& record, T value, Args... args)
{
    Logger::record("WRN", tag, record, value, args...);
}

template<typename T, typename... Args>
void Logger::error(const String& tag, const String& record, T value, Args... args)
{
    Logger::record("ERR", tag, record, value, args...);
}

template<typename T, typename... Args>
void Logger::printf(StringStream& ss, const char *s, T value, Args... args)
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
                ss << value;
                s += 1;
                Logger::printf(ss, s, args...); // call even when *s == 0 to detect extra arguments
                return;
            }
        }
        ss << *s++;
    }
}

class LoggerImpl : public Logger
{
public:
    ~LoggerImpl();

protected:
    virtual void append(const String& category, const String& tag, const String& record) override;
    static String getLocalTime(const String& format);

private:
    LoggerImpl();
    friend class Logger;
    std::ofstream stream_;
};

} // namespace rmsauth {

#endif // LOGGER_H
