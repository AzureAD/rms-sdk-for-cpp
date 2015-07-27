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

/*!
 *@brief RMS Error
 */
class RMSException : public std::exception {
public:

	/*!
	*@brief Type of exception
	*/
  enum ExceptionTypes {
    LogicError = 0
  };

  /*!
  *@brief Type of error
  */
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

  /*!
  *@brief
  */
  RMSException(const ExceptionTypes type,
               const int            error,
               const std::string  & message) _NOEXCEPT
    : type_(type), error_(error)
  {
    CopyMessage(message.c_str(), message.length());
  }

  /*!
  *@brief
  */
  RMSException(const ExceptionTypes type,
               const int            error,
               const char *const  & message) _NOEXCEPT
    : type_(type), error_(error)
  {
    CopyMessage(message, strlen(message));
  }

  /*!
  *@brief
  */
  virtual ~RMSException() _NOEXCEPT {}

  /*!
  *@brief
  */
  virtual const char* what() const _NOEXCEPT override {
    return message_;
  }

  /*!
  *@brief
  */
  virtual ExceptionTypes type() const _NOEXCEPT {
    return type_;
  }

  /*!
  *@brief
  */
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

/*!
*@brief Logic error
*/
class RMSLogicException : public RMSException {
public:

	/*!
	*@brief
	*/
  RMSLogicException(const int          error,
                    const std::string& message) _NOEXCEPT
    : RMSException(LogicError, error, message) {}

  /*!
  *@brief
  */
  RMSLogicException(const int          error,
                    const char *const& message) _NOEXCEPT
    : RMSException(LogicError, error, message) {}

  /*!
  *@brief
  */
  virtual ~RMSLogicException() _NOEXCEPT {}
};

/*!
*@brief Invalid argument
*/
class RMSInvalidArgumentException : public RMSLogicException {
public:

	/*!
	*@brief
	*/
  RMSInvalidArgumentException(const std::string& message) _NOEXCEPT
    : RMSLogicException(InvalidArgument, message) {}

  /*!
  *@brief
  */
  RMSInvalidArgumentException(const char *const& message) _NOEXCEPT
    : RMSLogicException(InvalidArgument, message) {}

  /*!
  *@brief
  */
  virtual ~RMSInvalidArgumentException() _NOEXCEPT {}
};

/*!
*@brief Null pointer
*/
class RMSNullPointerException : public RMSLogicException {
public:

	/*!
	*@brief
	*/
  RMSNullPointerException(const std::string& message) _NOEXCEPT
    : RMSLogicException(NullPointer, message) {}

  /*!
  *@brief
  */
  RMSNullPointerException(const char *const& message) _NOEXCEPT
    : RMSLogicException(NullPointer, message) {}

  /*!
  *@brief
  */
  virtual ~RMSNullPointerException() _NOEXCEPT {}
};

/*!
*@brief Not found
*/
class RMSNotFoundException : public RMSLogicException {
public:

	/*!
	*@brief
	*/
  RMSNotFoundException(const std::string& message) _NOEXCEPT
    : RMSLogicException(NullPointer, message) {}

  /*!
  *@brief
  */
  RMSNotFoundException(const char *const& message) _NOEXCEPT
    : RMSLogicException(NullPointer, message) {}

  /*!
  *@brief
  */
  virtual ~RMSNotFoundException() _NOEXCEPT {}
};

/*!
*@brief Network error
*/
class RMSNetworkException : public RMSLogicException {
public:

	/*!
	*@brief Possible seasons for network error
	*/
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

  /*!
  *@brief
  */
  RMSNetworkException(const std::string& message, Reason reason) _NOEXCEPT
    : RMSLogicException(NetworkError, message), reason_(reason) {}

  /*!
  *@brief
  */
  RMSNetworkException(const char *const& message, Reason reason) _NOEXCEPT
    : RMSLogicException(NetworkError, message), reason_(reason) {}

  /*!
  *@brief
  */
  virtual ~RMSNetworkException() _NOEXCEPT {}

  /*!
  *@brief
  */
  virtual Reason reason() const _NOEXCEPT {
    return reason_;
  }

private:

  Reason reason_; // additional reason for this error
};

/*!
*@brief Cryptographic error
*/
class RMSCryptographyException : public RMSLogicException {
public:

	/*!
	*@brief
	*/
  RMSCryptographyException(const std::string& message) _NOEXCEPT
    : RMSLogicException(NetworkError, message) {}

  /*!
  *@brief
  */
  RMSCryptographyException(const char *const& message) _NOEXCEPT
    : RMSLogicException(NetworkError, message) {}

  /*!
  *@brief
  */
  virtual ~RMSCryptographyException() _NOEXCEPT {}
};

/*!
*@brief Stream error
*/
class RMSStreamException : public RMSLogicException {
public:

	/*!
	*@brief
	*/
  RMSStreamException(const std::string& message) _NOEXCEPT
    : RMSLogicException(StreamError, message) {}

  /*!
  *@brief
  */
  RMSStreamException(const char *const& message) _NOEXCEPT
    : RMSLogicException(StreamError, message) {}

  /*!
  *@brief
  */
  virtual ~RMSStreamException() _NOEXCEPT {}
};

/*!
*@brief File error
*/
class RMSPFileException : public RMSLogicException {
public:

	/*!
	*@brief Reasons for file error
	*/
  enum Reason {
    NotPFile = 0,
    NotSupportedVersion,
    BadArguments,
  };

  /*!
  *@brief
  */
  RMSPFileException(const std::string& message, Reason reason) _NOEXCEPT
    : RMSLogicException(PFileError, message), reason_(reason) {}

  /*!
  *@brief
  */
  RMSPFileException(const char *const& message, Reason reason) _NOEXCEPT
    : RMSLogicException(PFileError, message), reason_(reason) {}

  /*!
  *@brief
  */
  virtual ~RMSPFileException() _NOEXCEPT {}

  /*!
  *@brief
  */
  virtual Reason reason() const _NOEXCEPT {
    return reason_;
  }

private:

  Reason reason_; // additional reason for this error
};

/*!
*@brief Rights error
*/
class RMSRightsException : public RMSLogicException {
public:

	/*!
	*@brief
	*/
  RMSRightsException(const std::string& message) _NOEXCEPT
    : RMSLogicException(PFileError, message) {}

  /*!
  *@brief
  */
  RMSRightsException(const char *const& message) _NOEXCEPT
    : RMSLogicException(PFileError, message) {}

  /*!
  *@brief
  */
  virtual ~RMSRightsException() _NOEXCEPT {}
};
} // namespace exceptions
} // namespace rmscore
#endif // _RMS_LIB_EXCEPTIONS_H
