#ifndef LOG4CPLUS_H
#define LOG4CPLUS_H

#include "StaticLogger.h"
#include <logger.h>
#include <configurator.h>
#include <consoleappender.h>
#include <fileappender.h>
#include <helpers/loglog.h>
#include <helpers/stringhelper.h>
#include <helpers/fileinfo.h>
#include <loggingmacros.h>
#include <initializer.h>

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

namespace rmscore {
namespace platform {
namespace staticlogger {

class Log4cplusImpl : public StaticLogger {
public:
    Log4cplusImpl();


protected:
    void trace(const std::string& record) override;
    void debug(const std::string& record) override;
    void info(const std::string& record) override;
    void warning(const std::string& record) override;
    void error(const std::string& record) override;
    void fatal(const std::string& record) override;
    void append(const string &prefix,
                const string &record) override;
private:
    Logger logger_;
    log4cplus::Initializer initializer;
};
} // namespace logger
} // namespace platform
} // namespace rmscore
#endif // LOG4CPLUS_H
