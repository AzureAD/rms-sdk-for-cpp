#ifndef _RMS_CRYPTO_STATICLOGGER_H_
#define _RMS_CRYPTO_STATICLOGGER_H_

#include <string>
#include <cstdio>
#include <mutex>
#include <memory>
#include <QProcessEnvironment>
#include "../Settings/IRMSCryptoEnvironmentImpl.h"

namespace rmscrypto {
namespace platform {
namespace staticlogger {
class StaticLogger{
    static const int max_length = 1024000;
private:
    friend class Log4cplusImpl;
    static StaticLogger& getInstance();
    static std::mutex m_;

protected:
    virtual void trace(const std::string& record)=0;
    virtual void debug(const std::string& record)=0;
    virtual void info(const std::string& record)=0;
    virtual void warning(const std::string& record)=0;
    virtual void error(const std::string& record)=0;
    virtual void fatal(const std::string& record)=0;
    virtual void append(const std::string& prefix,
                        const std::string& record) = 0;

public:
//    static void appenderHelper(const std::string& prefix, const std::string& record);
    template<typename ... Arguments>
    static void Append(const std::string& prefix,
                const std::string& record,
                Arguments ...      arguments) {
        auto cArgs =  sizeof ... (Arguments);

        auto env   = settings::IRMSCryptoEnvironmentImpl::Environment();
        if (!env || (env->LogOption() == api::IRMSCryptoEnvironment::LoggerOption::Never)) {
          return;
        }

        if (cArgs > 0) {
          std::string buff(StaticLogger::max_length, '-');
    #ifdef Q_OS_WIN32
          int num_bytes = sprintf_s(&buff[0], StaticLogger::max_length,
                                    record.c_str(), arguments ...);
    #else // ifdef Q_OS_WIN32
                # if defined(__clang__)
                    #  pragma clang diagnostic push
                    #  pragma clang diagnostic ignored "-Wformat-security"
                # endif // if defined(__clang__)
                # if defined(__GNUC__)
                    #  pragma GCC diagnostic ignored "-Wformat-security"
                # endif // if defined(__GNUC__)
          int num_bytes = std::snprintf(&buff[0], StaticLogger::max_length,
                                        record.c_str(), arguments ...);
                # if defined(__clang__)
                    #  pragma clang diagnostic pop
                # endif // if defined(__clang__)
    #endif              // ifdef Q_OS_WIN32

          if (num_bytes < StaticLogger::max_length) {
            buff.resize(num_bytes);
          }
          StaticLogger::getInstance().append(prefix, buff);
        } else {
          StaticLogger::getInstance().append(prefix, record);
        //}
      }
    }
    template<typename ... Arguments>
    static void Trace(const std::string& record, Arguments ... arguments){
        StaticLogger::Append("TRACE", record, arguments ...);
    }
    template<typename ... Arguments>
    static void Debug(const std::string& record, Arguments ... arguments){
        // read env var
        static QString ev = QProcessEnvironment::systemEnvironment().value(
          "RMS_DEBUG_LOG",
          "ON");

        // if set
        if (QString::compare(ev, "ON") == 0)
        {
          StaticLogger::Append("DEBUG", record, arguments ...);
        }
    }
    template<typename ... Arguments>
    static void Info(const std::string& record, Arguments ... arguments){
        StaticLogger::Append("INFO", record, arguments ...);
    }
    template<typename ... Arguments>
    static void Warning(const std::string& record, Arguments ... arguments){
        StaticLogger::Append("WRN", record, arguments ...);
    }
    template<typename ... Arguments>
    static void Error(const std::string& record, Arguments ... arguments){
        StaticLogger::Append("ERROR", record, arguments ...);
    }
    template<typename ... Arguments>
    static void Fatal(const std::string& record, Arguments ... arguments){
        StaticLogger::Append("FATAL", record, arguments ...);
    }
    //~StaticLogger();
};
} // namespace logger4
} // namespace platform
} // namespace rmscore

#endif // _RMS_CRYPTO_STATICLOGGER_H_
