/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <cstdio>
#include <QDebug>

class Logger
{
    static const int max_length = 1024000;
public:
    template <typename... Arguments>
    static void Append(const std::string& prefix, const std::string& record, Arguments... arguments) {
        auto cArgs =  sizeof...(Arguments);
        if (cArgs > 0) {
            std::string buff(max_length, '-');
#ifdef Q_OS_WIN32
            int num_bytes = sprintf_s (&buff[0], max_length, record.c_str(), arguments...);
#else
            #if defined(__clang__)
                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wformat-security"
            #endif
            #if defined(__GNUC__)
                #pragma GCC diagnostic ignored "-Wformat-security"
            #endif
            int num_bytes = std::snprintf (&buff[0], max_length, record.c_str(), arguments...);
            #if defined(__clang__)
                #pragma clang diagnostic pop
            #endif
#endif
            if (num_bytes < max_length) {
                buff.resize(num_bytes);
            }
            Logger::instance().append(prefix, buff);
        } else {
            Logger::instance().append(prefix, record);
        }
    }

    template <typename... Arguments>
    static void Info(const std::string& record, Arguments... arguments) {
        Logger::Append("INF", record, arguments...);
    }

    template <typename... Arguments>
    static void Warning(const std::string& record, Arguments... arguments) {
        Logger::Append("WRN", record, arguments...);
    }

    template <typename... Arguments>
    static void Error(const std::string& record, Arguments... arguments) {
        Logger::Append("ERR", record, arguments...);
    }

#ifndef QT_DEBUG
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wunused-parameter"
    #endif
    #if defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-parameter"
    #endif
#endif
    template <typename... Arguments>
    static void Debug(const std::string& record, Arguments... arguments) {
#ifdef QT_DEBUG
        Logger::Append("DBG", record, arguments...);
#endif
    }
#ifndef QT_DEBUG
    #if defined(__GNUC__)
        #pragma GCC diagnostic pop
    #endif
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif
#endif

    virtual ~Logger(){}

protected:
    virtual void append(const std::string& prefix, const std::string& record) = 0;

private:
    static Logger& instance();
};

#endif // _LOGGER_H_
