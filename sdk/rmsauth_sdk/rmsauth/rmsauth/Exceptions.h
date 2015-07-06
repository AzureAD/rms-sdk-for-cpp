/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef EXCEPTIONS
#define EXCEPTIONS
#include "types.h"
#include <exception>

#ifndef _NOEXCEPT
#if __GNUC__ >= 4
#define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT
#endif
#endif

namespace rmsauth {

class Exception : public std::exception
{
public:
    Exception(const String& error)
        : std::exception()
        , error_(error)
    {}

    Exception(const String& error, const String& message)
        : std::exception()
        , error_(error)
        , message_(message)
    {}

    virtual const char* what() const _NOEXCEPT {return error_.c_str();}
    virtual const String& error() const {return error_;}
    virtual const String& message() const {return message_;}

private:
    String error_;
    String message_;
};

class RmsauthException : public Exception
{
public:
//    using Exception::Exception;
    RmsauthException(const String& error, const String& message) : Exception(error, message){}
    RmsauthException(const String& error)  : RmsauthException(error, ""){}
};

class RmsauthServiceException: public RmsauthException
{
public:
//    using RmsauthException::RmsauthException;
    RmsauthServiceException(const String& error, const String& message) : RmsauthException(error, message)  {}
    RmsauthServiceException(const String& error)  : RmsauthServiceException(error, ""){}

};

class RmsauthUserMismatchException: public RmsauthException
{
public:
//    using RmsauthException::RmsauthException;
    RmsauthUserMismatchException(const String& error, const String& message) : RmsauthException(error, message){}
    RmsauthUserMismatchException(const String& error) : RmsauthUserMismatchException(error, ""){}

};

class RmsauthParsingException: public RmsauthException
{
public:
//    using RmsauthException::RmsauthException;
    RmsauthParsingException(const String& error, const String& message) : RmsauthException(error, message){}
    RmsauthParsingException(const String& error) : RmsauthParsingException(error, ""){}

};

class RmsauthJsonParsingException: public RmsauthParsingException
{
public:
//    using RmsauthParsingException::RmsauthParsingException;
    RmsauthJsonParsingException(const String& error, const String& message) : RmsauthParsingException(error, message){}
    RmsauthJsonParsingException(const String& error) : RmsauthJsonParsingException(error, ""){}
};

class IllegalArgumentException : public Exception
{
public:
//    using Exception::Exception;
    IllegalArgumentException(const String& error, const String& message) : Exception(error, message){}
    IllegalArgumentException(const String& error) : IllegalArgumentException(error, ""){}
};

} // namespace rmsauth {

#endif // EXCEPTIONS

