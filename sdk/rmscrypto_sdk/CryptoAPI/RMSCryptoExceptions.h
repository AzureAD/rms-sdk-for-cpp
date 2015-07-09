/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _CRYPTO_STREAMS_LIB_EXCEPTIONS_H
#define _CRYPTO_STREAMS_LIB_EXCEPTIONS_H
#include <cstring>
#include <string>
#include <exception>
#include <algorithm>

#ifndef _NOEXCEPT
# if __GNUC__ >= 4
#  define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT
# endif // if __GNUC__ >= 4
#endif  // ifndef _NOEXCEPT

namespace rmscrypto {
namespace exceptions {
#define MAX_EXCEPTION_MESSAGE_LENGTH 255
class RMSCryptoException : public std::exception {
public:

  enum ExceptionTypes {
    LogicError = 0,
    IOError
  };
  enum ErrorTypes {
    InvalidArgument = 0,
    NullPointer,
    OperationUnavailable,
    Insufficientbuffer,
    NotImplemented,
    SecretKeyException,
    UnknownError
  };

  RMSCryptoException(const ExceptionTypes type,
                     const int            error,
                     const std::string  & message) _NOEXCEPT
    : type_(type), error_(error)
  {
    CopyMessage(message.c_str(), message.length());
  }

  RMSCryptoException(const ExceptionTypes type,
                     const int            error,
                     const char *const  & message) _NOEXCEPT
    : type_(type), error_(error)
  {
    CopyMessage(message, strlen(message));
  }

  virtual ~RMSCryptoException() _NOEXCEPT {}

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

class RMSCryptoLogicException : public RMSCryptoException {
public:

  RMSCryptoLogicException(const int          error,
                          const std::string& message) _NOEXCEPT
    : RMSCryptoException(LogicError, error, message) {}

  RMSCryptoLogicException(const int          error,
                          const char *const& message) _NOEXCEPT
    : RMSCryptoException(LogicError, error, message) {}

  virtual ~RMSCryptoLogicException() _NOEXCEPT {}
};

class RMSCryptoIOException : public RMSCryptoException {
public:

  RMSCryptoIOException(const int          error,
                       const std::string& message) _NOEXCEPT
    : RMSCryptoException(IOError, error, message) {}

  RMSCryptoIOException(const int          error,
                       const char *const& message) _NOEXCEPT
    : RMSCryptoException(IOError, error, message) {}

  virtual ~RMSCryptoIOException() _NOEXCEPT {}
};

class RMSCryptoIOKeyException : public RMSCryptoIOException {
public:

  RMSCryptoIOKeyException(const std::string& message, int code) _NOEXCEPT
    : RMSCryptoIOException(SecretKeyException, message), code_(code) {}

  RMSCryptoIOKeyException(const char *const& message, int code) _NOEXCEPT
    : RMSCryptoIOException(SecretKeyException, message), code_(code) {}

  virtual ~RMSCryptoIOKeyException() _NOEXCEPT {}

  virtual int code() const _NOEXCEPT {
    return code_;
  }

private:

  int code_; // additional reason for this error
};


class RMSCryptoInvalidArgumentException : public RMSCryptoLogicException {
public:

  RMSCryptoInvalidArgumentException(const std::string& message) _NOEXCEPT
    : RMSCryptoLogicException(InvalidArgument, message) {}

  RMSCryptoInvalidArgumentException(const char *const& message) _NOEXCEPT
    : RMSCryptoLogicException(InvalidArgument, message) {}

  virtual ~RMSCryptoInvalidArgumentException() _NOEXCEPT {}
};

class RMSCryptoNullPointerException : public RMSCryptoLogicException {
public:

  RMSCryptoNullPointerException(const std::string& message) _NOEXCEPT
    : RMSCryptoLogicException(NullPointer, message) {}

  RMSCryptoNullPointerException(const char *const& message) _NOEXCEPT
    : RMSCryptoLogicException(NullPointer, message) {}

  virtual ~RMSCryptoNullPointerException() _NOEXCEPT {}
};

class RMSCryptoInsufficientBufferException : public RMSCryptoLogicException {
public:

  RMSCryptoInsufficientBufferException(const std::string& message) _NOEXCEPT
    : RMSCryptoLogicException(NullPointer, message) {}

  RMSCryptoInsufficientBufferException(const char *const& message) _NOEXCEPT
    : RMSCryptoLogicException(NullPointer, message) {}

  virtual ~RMSCryptoInsufficientBufferException() _NOEXCEPT {}
};

class RMSCryptoNotImplementedException : public RMSCryptoLogicException {
public:

  RMSCryptoNotImplementedException(const std::string& message) _NOEXCEPT
    : RMSCryptoLogicException(NotImplemented, message) {}

  RMSCryptoNotImplementedException(const char *const& message) _NOEXCEPT
    : RMSCryptoLogicException(NotImplemented, message) {}

  virtual ~RMSCryptoNotImplementedException() _NOEXCEPT {}
};
} // namespace exceptions
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_EXCEPTIONS_H
