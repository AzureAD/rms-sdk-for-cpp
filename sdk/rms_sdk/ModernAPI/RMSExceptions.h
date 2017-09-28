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
#include <cstring>
#include <exception>
#include <algorithm>

#ifndef _NOEXCEPT
# if __GNUC__ >= 4
#  define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT
# endif // if __GNUC__ >= 4
#endif  // ifndef _NOEXCEPT

namespace rmscore {
namespace exceptions {
#define MAX_EXCEPTION_MESSAGE_LENGTH 255
class RMSException : public std::exception {
public:

  enum class ExceptionTypes : int {
    LogicError = 0
  };
  enum class ErrorTypes : int {
    InvalidArgument = 0,
    NullPointer,
    NotFound,
    NetworkError,
    CryptoError,
    StreamError,
    PFileError,
    RightsError,
    OfficeFileError,
    NotSupported,
    FileError,
  };

  RMSException(const ExceptionTypes type,
               const int            error,
               const std::string  & message) _NOEXCEPT
    : type_(type), error_(error)
  {
    CopyMessage(message.c_str(), message.length());
  }

  RMSException(const ExceptionTypes type,
               const int            error,
               const char *const  & message) _NOEXCEPT
    : type_(type), error_(error)
  {
    CopyMessage(message, strlen(message));
  }

  virtual ~RMSException() _NOEXCEPT {}

  virtual const char* what() const _NOEXCEPT override {
    return message_;
  }

  virtual ExceptionTypes type() const _NOEXCEPT {
    return type_;
  }

  virtual int error() const _NOEXCEPT {
    return error_;
  }

private:

  void CopyMessage(const char *msg, const size_t len) {
    size_t mlen =
      (std::min)(len, static_cast<const size_t>(MAX_EXCEPTION_MESSAGE_LENGTH - 1));

    memset(message_, 0, sizeof(message_));

    if (mlen > 0) {
  #ifdef Q_OS_WIN32
      memcpy_s(message_, mlen, msg, mlen);
  #else // ifdef Q_OS_WIN32
      memcpy(message_, msg, mlen);
  #endif // ifdef Q_OS_WIN32
    }
  }

  ExceptionTypes type_;
  int  error_;
  char message_[MAX_EXCEPTION_MESSAGE_LENGTH];
};

class RMSLogicException : public RMSException {
public:

  RMSLogicException(const ErrorTypes   error,
                    const std::string& message) _NOEXCEPT
    : RMSException(ExceptionTypes::LogicError, static_cast<int>(error), message) {}

  RMSLogicException(const ErrorTypes   error,
                    const char *const& message) _NOEXCEPT
    : RMSException(ExceptionTypes::LogicError, static_cast<int>(error), message) {}

  virtual ~RMSLogicException() _NOEXCEPT {}
};

class RMSInvalidArgumentException : public RMSLogicException {
public:

  RMSInvalidArgumentException(const std::string& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::InvalidArgument, message) {}

  RMSInvalidArgumentException(const char *const& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::InvalidArgument, message) {}

  virtual ~RMSInvalidArgumentException() _NOEXCEPT {}
};

class RMSNullPointerException : public RMSLogicException {
public:

  RMSNullPointerException(const std::string& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::NullPointer, message) {}

  RMSNullPointerException(const char *const& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::NullPointer, message) {}

  virtual ~RMSNullPointerException() _NOEXCEPT {}
};

class RMSNotFoundException : public RMSLogicException {
public:

  RMSNotFoundException(const std::string& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::NullPointer, message) {}

  RMSNotFoundException(const char *const& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::NullPointer, message) {}

  virtual ~RMSNotFoundException() _NOEXCEPT {}
};

class RMSNetworkException : public RMSLogicException {
public:

  enum class Reason : int {
    ServerError = 0,
    UserNotConsented,
    ServiceNotAvailable,
    OnPremNotSupported,
    InvalidPL,
    ServiceDisabled,
    DeviceRejected,
    NeedsOnline,
    CancelledByUser
  };


  RMSNetworkException(const std::string& message, Reason reason) _NOEXCEPT
    : RMSLogicException(ErrorTypes::NetworkError, message), reason_(reason) {}

  RMSNetworkException(const char *const& message, Reason reason) _NOEXCEPT
    : RMSLogicException(ErrorTypes::NetworkError, message), reason_(reason) {}

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
    : RMSLogicException(ErrorTypes::NetworkError, message) {}

  RMSCryptographyException(const char *const& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::NetworkError, message) {}

  virtual ~RMSCryptographyException() _NOEXCEPT {}
};
class RMSStreamException : public RMSLogicException {
public:

  RMSStreamException(const std::string& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::StreamError, message) {}

  RMSStreamException(const char *const& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::StreamError, message) {}

  virtual ~RMSStreamException() _NOEXCEPT {}
};
class RMSPFileException : public RMSLogicException {
public:

  enum class Reason : int {
    NotPFile = 0,
    NotSupportedVersion,
    BadArguments,
    CorruptFile,
    AlreadyProtected,
  };

  RMSPFileException(const std::string& message, Reason reason) _NOEXCEPT
    : RMSLogicException(ErrorTypes::PFileError, message), reason_(reason) {}

  RMSPFileException(const char *const& message, Reason reason) _NOEXCEPT
    : RMSLogicException(ErrorTypes::PFileError, message), reason_(reason) {}

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
    : RMSLogicException(ErrorTypes::RightsError, message) {}

  RMSRightsException(const char *const& message) _NOEXCEPT
    : RMSLogicException(ErrorTypes::RightsError, message) {}

  virtual ~RMSRightsException() _NOEXCEPT {}
};

class RMSOfficeFileException : public RMSLogicException {
public:

  enum class Reason : int {
    NotOfficeFile = 0,
    BadArguments,
    CorruptFile,
    NotProtected,
    NonRMSProtected,
    AlreadyProtected,
    CompoundFileError,
    Unknown
  };

  RMSOfficeFileException(const std::string& message, Reason reason) _NOEXCEPT
    : RMSLogicException(ErrorTypes::OfficeFileError, message), reason_(reason) {}

  RMSOfficeFileException(const char *const& message, Reason reason) _NOEXCEPT
    : RMSLogicException(ErrorTypes::OfficeFileError, message), reason_(reason) {}

  virtual ~RMSOfficeFileException() _NOEXCEPT {}

  virtual Reason reason() const _NOEXCEPT {
    return reason_;
  }

private:
  Reason reason_; // additional reason for this error
};

} // namespace exceptions
} // namespace rmscore
#endif // _RMS_LIB_EXCEPTIONS_H
