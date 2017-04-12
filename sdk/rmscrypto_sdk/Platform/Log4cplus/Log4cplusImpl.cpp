#ifdef QTFRAMEWORK
#include <Log4cplusImpl.h>
#include <iostream>

namespace rmscrypto {
namespace platform {
namespace staticlogger{

StaticLogger& StaticLogger::getInstance(){
    static Log4cplusImpl instance;
    return instance;
}

Log4cplusImpl::Log4cplusImpl() {
    try{
        helpers::LogLog::getLogLog()->setInternalDebugging(true);
        PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("Log4cplus_crypto.properties"));
        this->logger_ = Logger::getInstance(LOG4CPLUS_TEXT("rmscrypto_sdk_log"));
    }
    catch(...){
        std::cout << LOG4CPLUS_TEXT("Exception...") << endl;
                LOG4CPLUS_FATAL(Logger::getRoot(), "Exception occured...");
    }
}

void Log4cplusImpl::append(const std::string &prefix, const std::string &record){
    if(prefix.compare("TRACE")==0){
            Log4cplusImpl::trace(record);
    }
    else if(prefix.compare("DEBUG")==0){
        Log4cplusImpl::debug(record);
    }
    else if(prefix.compare("INFO")==0){
        Log4cplusImpl::info(record);
    }
    else if(prefix.compare("WRN")==0){
        Log4cplusImpl::warning(record);
    }
    else if(prefix.compare("ERROR")==0){
        Log4cplusImpl::error(record);
    }
    else if(prefix.compare("FATAL")==0){
        Log4cplusImpl::fatal(record);
    }
}

void Log4cplusImpl::trace(const std::string& record){
    LOG4CPLUS_TRACE(this->logger_,record.c_str());
}

void Log4cplusImpl::debug(const std::string& record){
    LOG4CPLUS_DEBUG(this->logger_,record.c_str());
}

void Log4cplusImpl::info(const std::string& record){
    LOG4CPLUS_INFO(this->logger_,record.c_str());
}

void Log4cplusImpl::warning(const std::string& record){
    LOG4CPLUS_WARN(this->logger_,record.c_str());
}

void Log4cplusImpl::error(const std::string& record){
    LOG4CPLUS_ERROR(this->logger_,record.c_str());
}

void Log4cplusImpl::fatal(const std::string& record){
    LOG4CPLUS_FATAL(this->logger_,record.c_str());
}
} // namespace staticlogger
} // namespace platform
} // namespace rmscore
#endif // QTFRAMEWORK
