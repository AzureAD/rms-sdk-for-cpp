#ifndef FILE_SDK_EXCEPTIONS_H
#define FILE_SDK_EXCEPTIONS_H

#include <exception>

#ifndef _NOEXCEPT
#if __GNUC__ >= 4
#define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT
#endif
#endif

namespace mip {

class Exception : public std::exception {
public:
  Exception(const std::string& error) _NOEXCEPT
      : std::exception(),
        mError(error) {}

  Exception(const std::string& error, const std::string& message) _NOEXCEPT
      : std::exception(),
        mError(error),
        mMessage(message) {}

  virtual const char* what() const _NOEXCEPT { return mError.c_str(); }
  virtual const std::string& error() const { return mError; }
  virtual const std::string& message() const { return mMessage; }

private:
  std::string mError;
  std::string mMessage;
};

class IOException : public Exception
{
public:
  IOException(const std::string& error, const std::string& message) _NOEXCEPT : Exception(error, message) {}
  IOException(const std::string& error) _NOEXCEPT : IOException(error, "") {}
};

} // namespace mip

#endif // FILE_SDK_EXCEPTIONS_H
