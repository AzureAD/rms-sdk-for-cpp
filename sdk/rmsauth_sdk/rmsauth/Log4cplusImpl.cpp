#ifdef RMSAUTH_LIBRARY
#include <Log4cplusImpl.h>
#include <Exceptions.h>
#include <iostream>

namespace rmsauth {

StaticLogger& StaticLogger::getInstance(){
    static Log4cplusImpl instance;
    return instance;
}

void StaticLogger::printf(StringStream& ss, const char *s)
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

Log4cplusImpl::Log4cplusImpl() {
    try{
        helpers::LogLog::getLogLog()->setInternalDebugging(true);
        PropertyConfigurator::doConfigure("Log4cplus_auth.properties");
        this->logger_ = Logger::getInstance(LOG4CPLUS_TEXT("rmsauth_sdk_log"));
    }
    catch(...){
        std::cout << LOG4CPLUS_TEXT("Exception...") << endl;
                LOG4CPLUS_FATAL(Logger::getRoot(), "Exception occured...");
    }
}

void Log4cplusImpl::append(const std::string& prefix,
                           const std::string& tag,
                           const std::string& record){
    if(prefix.compare("TRACE")==0){
            Log4cplusImpl::trace(tag, record);
    }
    else if(prefix.compare("DEBUG")==0){
        Log4cplusImpl::debug(tag, record);
    }
    else if(prefix.compare("INFO")==0){
        Log4cplusImpl::info(tag, record);
    }
    else if(prefix.compare("WRN")==0){
        Log4cplusImpl::warning(tag, record);
    }
    else if(prefix.compare("ERROR")==0){
        Log4cplusImpl::error(tag, record);
    }
    else if(prefix.compare("FATAL")==0){
        Log4cplusImpl::fatal(tag, record);
    }
}

void Log4cplusImpl::trace(const std::string& tag, const std::string& record){
    std::stringstream ss;
    ss << tag << ": " << record;
    LOG4CPLUS_TRACE(this->logger_,ss.str().c_str());
}

void Log4cplusImpl::debug(const std::string& tag, const std::string& record){
    std::stringstream ss;
    ss << tag << ": " << record;
    LOG4CPLUS_DEBUG(this->logger_,ss.str().c_str());
}

void Log4cplusImpl::info(const std::string& tag, const std::string& record){
    std::stringstream ss;
    ss << tag << ": " << record;
    LOG4CPLUS_INFO(this->logger_,ss.str().c_str());
}

void Log4cplusImpl::warning(const std::string& tag, const std::string& record){
    std::stringstream ss;
    ss << tag << ": " << record;
    LOG4CPLUS_WARN(this->logger_,ss.str().c_str());
}

void Log4cplusImpl::error(const std::string& tag, const std::string& record){
    std::stringstream ss;
    ss << tag << ": " << record;
    LOG4CPLUS_ERROR(this->logger_,ss.str().c_str());
}

void Log4cplusImpl::fatal(const std::string& tag, const std::string& record){
    std::stringstream ss;
    ss << tag << ": " << record;
    LOG4CPLUS_FATAL(this->logger_,ss.str().c_str());
}
} // namespace rmsauth
#endif // RMSAUTH_LIBRARY
