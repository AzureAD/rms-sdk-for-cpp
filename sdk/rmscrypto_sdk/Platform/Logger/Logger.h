/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_LOGGER_H_
#define _CRYPTO_STREAMS_LIB_LOGGER_H_

#include <string>
#include <cstdio>
#include <QDebug>
#include <QProcessEnvironment>

class LoggerCrypto
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
            LoggerCrypto::instance().append(prefix, buff);
        } else {
            LoggerCrypto::instance().append(prefix, record);
        }
    }

    template <typename... Arguments>
    static void Info(const std::string& record, Arguments... arguments) {
        LoggerCrypto::Append("INF", record, arguments...);
    }

    template <typename... Arguments>
    static void Warning(const std::string& record, Arguments... arguments) {
        LoggerCrypto::Append("WRN", record, arguments...);
    }

    template <typename... Arguments>
    static void Error(const std::string& record, Arguments... arguments) {
        LoggerCrypto::Append("ERR", record, arguments...);
    }

    template <typename... Arguments>
    static void Hidden(const std::string& record, Arguments... arguments) {
        // read env var
        static QString ev = QProcessEnvironment::systemEnvironment().value("RMS_HIDDEN_LOG", "OFF");
        // if set
        if(QString::compare(ev, "ON") == 0)
        {
            LoggerCrypto::Append("HDN", record, arguments...);
        }
    }

    virtual ~LoggerCrypto(){}

protected:
    virtual void append(const std::string& prefix, const std::string& record) = 0;

private:
    static LoggerCrypto& instance();
};

using Logger = LoggerCrypto;

#endif // _CRYPTO_STREAMS_LIB_LOGGER_H_
