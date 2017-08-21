#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <memory>
#include <string>

#include "macros.h"

namespace base {

// Defines the string format used for exceptions
// TODO: find a way to remove typename to reduce binary size
template<typename ... Args>
std::string StringFormat(const char* format, Args... args)
{
  int size = SNPRINTF(nullptr, 0, format, args...) + 1;
  std::unique_ptr<char[]> buf(new char[size]);
  int ret = SNPRINTF(buf.get(), size, format, args...);
  return std::string(buf.get(), buf.get() + size - 1);
}

template<typename ... Args>
std::string StringFormat(const std::string& format, Args... args)
{
  return StringFormat(format.c_str(), args...);
}

// Tries to parse the given string into a boolean value. Case does not matter for the input string.
// IMPORTANT: Return value only indicates whether parsing was successful or not, the actual parsed
// value is reutrned in out parameter result.
bool TryParseBool(const std::string& str, bool& result);

// Parses the given string into a boolean value. Throws std::invalid_argument if the provided
// value cannot be parsed into a bool. Use TryParseBool above if exception is undesirable for
// invalid input.
bool ParseBool(const std::string& str);

// Convert a utf8 string to wstring.
std::wstring ConvertStringToWString(const std::string& str);
}  // namespace base

#endif  // BASE_STRING_UTILS_H_
