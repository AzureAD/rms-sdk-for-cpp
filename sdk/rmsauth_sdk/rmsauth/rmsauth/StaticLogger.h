#ifndef _RMS_AUTH_STATICLOGGER_H_
#define _RMS_AUTH_STATICLOGGER_H_

#include <string>
#include <cstdio>
#include <mutex>
#include <memory>
#include <QProcessEnvironment>
#include <fstream>
#include "types.h"
#include "rmsauthExport.h"
#include "IRMSAuthEnvironment.h"

namespace rmsauth {

class StaticLogger{
    static const int max_length = 1024000;
private:
    friend class Log4cplusImpl;
    static StaticLogger& getInstance();
    static std::mutex m_;
    static void printf(StringStream& ss, const char *s);
    template<typename T, typename... Args>
    static void printf(StringStream& ss, const char *s, T value, Args... args);

protected:
    virtual void trace(const std::string& tag,const std::string& record)=0;
    virtual void debug(const std::string& tag,const std::string& record)=0;
    virtual void info(const std::string& tag,const std::string& record)=0;
    virtual void warning(const std::string& tag,const std::string& record)=0;
    virtual void error(const std::string& tag,const std::string& record)=0;
    virtual void fatal(const std::string& tag,const std::string& record)=0;
    virtual void append(const std::string& prefix,
                        const std::string& tag,
                        const std::string& record) = 0;

public:
    static void Append(const std::string& category, const std::string& tag, const std::string& record){
        auto env = RMSAuthEnvironment();
        if (!env || (env->LogOption() == IRMSAuthEnvironment::LoggerOption::Never)) {
          return;
        }
        StaticLogger::getInstance().append(category, tag, record);
    }
    static void Trace(const String& tag, const String& record){
        StaticLogger::Append("TRACE", tag, record);
    }
    static void Debug(const String& tag, const String& record){
        // read env var
        static QString ev = QProcessEnvironment::systemEnvironment().value(
          "RMS_DEBUG_LOG",
          "ON");

        // if set
        if (QString::compare(ev, "ON") == 0)
        {
          StaticLogger::Append("DEBUG", tag, record);
        }
    }
    static void Info(const String& tag, const String& record){
        StaticLogger::Append("INFO", tag, record);
    }
    static void Warning(const String& tag, const String& record){
        StaticLogger::Append("WRN", tag, record);
    }
    static void Error(const String& tag, const String& record){
        StaticLogger::Append("ERROR", tag, record);
    }
    static void Fatal(const String& tag, const String& record){
        StaticLogger::Append("Fatal", tag, record);
    }

//    static void appenderHelper(const std::string& prefix, const std::string& record);
    template<typename T, typename... Args>
    static void Append(const std::string& category,
                       const std::string& tag,
                       const std::string& record,
                       T value, Args... args) {
        auto env = RMSAuthEnvironment();
        if (!env || (env->LogOption() == IRMSAuthEnvironment::LoggerOption::Never)) {
          return;
        }

        StringStream ss;
        StaticLogger::printf(ss, record.c_str(), value, args...);
        StaticLogger::getInstance().append(category, tag, ss.str());
    }
    template<typename T, typename... Args>
    static void Trace(const std::string& tag, const std::string& record, T value, Args... args){
        StaticLogger::Append("TRACE", tag, record, value, args...);
    }
    template<typename T, typename... Args>
    static void Debug(const std::string& tag, const std::string& record, T value, Args... args){
        // read env var
        static QString ev = QProcessEnvironment::systemEnvironment().value(
          "RMS_DEBUG_LOG",
          "ON");

        // if set
        if (QString::compare(ev, "ON") == 0)
        {
          StaticLogger::Append("DEBUG", tag, record, value, args...);
        }
    }
    template<typename T, typename... Args>
    static void Info(const std::string& tag, const std::string& record, T value, Args... args){
        StaticLogger::Append("INFO", tag, record, value, args...);
    }
    template<typename T, typename... Args>
    static void Warning(const std::string& tag, const std::string& record, T value, Args... args){
        StaticLogger::Append("WRN", tag, record, value, args...);
    }
    template<typename T, typename... Args>
    static void Error(const std::string& tag, const std::string& record, T value, Args... args){
        StaticLogger::Append("ERROR", tag, record, value, args...);
    }
    template<typename T, typename... Args>
    static void Fatal(const std::string& tag, const std::string& record, T value, Args... args){
        StaticLogger::Append("FATAL", tag, record, value, args...);
    }
    //~StaticLogger();
};
template<typename T, typename... Args>
void StaticLogger::printf(StringStream& ss, const char *s, T value, Args... args)
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
                StaticLogger::printf(ss, s, args...); // call even when *s == 0 to detect extra arguments
                return;
            }
        }
        ss << *s++;
    }
}
} // namespace rmsauth

#endif // RMS_AUTH_STATICLOGGER_H
