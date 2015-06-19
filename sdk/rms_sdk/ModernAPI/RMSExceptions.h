/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_EXCEPTIONS_H
#define _RMS_LIB_EXCEPTIONS_H
#include <string>
#include <exception>

#ifndef _NOEXCEPT
#if __GNUC__ >= 4
#define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT
#endif
#endif

namespace rmscore {
namespace exceptions {
class RMSException : public std::exception {
public:

  enum ExceptionTypes {
    LogicError = 0
  };
  enum ErrorTypes {
    InvalidArgument = 0,
    NullPointer,
    NotFound,
    NetworkError,
    CryptoError,
    StreamError,
    PFileError,
    RightsError
  };

  RMSException(const ExceptionTypes type,
               const int            error,
               const std::string  & message) _NOEXCEPT
    : type_(type), error_(error), message_(message.c_str())
  {}

  RMSException(const ExceptionTypes type,
               const int            error,
               const char *const  & message) _NOEXCEPT
    : type_(type), error_(error), message_(message)
  {}

  virtual ~RMSException() _NOEXCEPT {}

  virtual const char * what() const _NOEXCEPT override {
    return message_;
  }

  virtual ExceptionTypes type() const _NOEXCEPT {
    return type_;
  }

  virtual int error() const _NOEXCEPT {
    return error_;
  }

private:

  ExceptionTypes type_;
  int error_;
  const char * message_;
};

class RMSLogicException : public RMSException {
public:

  RMSLogicException(const int          error,
                    const std::string& message) _NOEXCEPT
    : RMSException(LogicError, error, message) {}

  RMSLogicException(const int          error,
                    const char *const& message) _NOEXCEPT
    : RMSException(LogicError, error, message) {}

  virtual ~RMSLogicException() _NOEXCEPT {}
};

class RMSInvalidArgumentException : public RMSLogicException {
public:

  RMSInvalidArgumentException(const std::string& message) _NOEXCEPT
    : RMSLogicException(InvalidArgument, message) {}

  RMSInvalidArgumentException(const char *const& message) _NOEXCEPT
    : RMSLogicException(InvalidArgument, message) {}

  virtual ~RMSInvalidArgumentException() _NOEXCEPT {}
};

class RMSNullPointerException : public RMSLogicException {
public:

  RMSNullPointerException(const std::string& message) _NOEXCEPT
    : RMSLogicException(NullPointer, message) {}

  RMSNullPointerException(const char *const& message) _NOEXCEPT
    : RMSLogicException(NullPointer, message) {}

  virtual ~RMSNullPointerException() _NOEXCEPT {}
};

class RMSNotFoundException : public RMSLogicException {
public:

  RMSNotFoundException(const std::string& message) _NOEXCEPT
    : RMSLogicException(NullPointer, message) {}

  RMSNotFoundException(const char *const& message) _NOEXCEPT
    : RMSLogicException(NullPointer, message) {}

  virtual ~RMSNotFoundException() _NOEXCEPT {}
};

class RMSNetworkException : public RMSLogicException {
public:

  enum Reason {
    ServerError = 0,
    UserNotConsented,
    ServiceNotAvailable,
    OnPremNotSupported,
    InvalidPL,
    ServiceDisabled,
    DeviceRejected,
    NeedsOnline
  };


  RMSNetworkException(const std::string& message, Reason reason) _NOEXCEPT
    : RMSLogicException(NetworkError, message), reason_(reason) {}

  RMSNetworkException(const char *const& message, Reason reason) _NOEXCEPT
    : RMSLogicException(NetworkError, message), reason_(reason) {}

  virtual ~RMSNetworkException() _NOEXCEPT {}

  virtual Reason reason() const _NOEXCEPT {
    return reason_;
  }

private:

  Reason reason_; // additional reason for this error
};

class RMSCryptographyException : public RMSLogicException {
public:

  RMSCryptographyException(const std::string& message) _NOEXCEPT
    : RMSLogicException(NetworkError, message) {}

  RMSCryptographyException(const char *const& message) _NOEXCEPT
    : RMSLogicException(NetworkError, message) {}

  virtual ~RMSCryptographyException() _NOEXCEPT {}
};
class RMSStreamException : public RMSLogicException {
public:

  RMSStreamException(const std::string& message) _NOEXCEPT
    : RMSLogicException(StreamError, message) {}

  RMSStreamException(const char *const& message) _NOEXCEPT
    : RMSLogicException(StreamError, message) {}

  virtual ~RMSStreamException() _NOEXCEPT {}
};
class RMSPFileException : public RMSLogicException {
public:

  enum Reason {
    NotPFile = 0,
    NotSupportedVersion,
    BadArguments,
  };

  RMSPFileException(const std::string& message, Reason reason) _NOEXCEPT
    : RMSLogicException(PFileError, message), reason_(reason) {}

  RMSPFileException(const char *const& message, Reason reason) _NOEXCEPT
    : RMSLogicException(PFileError, message), reason_(reason) {}

  virtual ~RMSPFileException() _NOEXCEPT {}

  virtual Reason reason() const _NOEXCEPT {
    return reason_;
  }

private:

  Reason reason_; // additional reason for this error
};

class RMSRightsException : public RMSLogicException {
public:

  RMSRightsException(const std::string& message) _NOEXCEPT
    : RMSLogicException(PFileError, message) {}

  RMSRightsException(const char *const& message) _NOEXCEPT
    : RMSLogicException(PFileError, message) {}

  virtual ~RMSRightsException() _NOEXCEPT {}
};
} // namespace exceptions
} // namespace rmscore
#endif // _RMS_LIB_EXCEPTIONS_H
