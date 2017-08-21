#include "string_utils.h"

#include <algorithm>
#include <cctype>
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <string>

using std::string;
using std::wstring;

namespace base {

bool TryParseBool(const string& str, bool& result) {
  // Perf nit: We do not need a new string that is lower case. We can do
  // a case insensitive comparison with "true" or "false". That has two (nit)
  // perf benefits: another string is not allocated, time complexity is always
  // O(1) since parsing will only do maximum 5 comparisions ("false").
  // TODO: Find a portable way to do case insensitive comparison.
  string s(str);
  transform(s.begin(), s.end(), s.begin(), ::tolower);
  // Currently we do not ignore leading and trailing whitespace.
  // Add that if needed, potentially controlled by a flag passed to the function.
  if  (s == "true") {
    result = true;
    return true;
  } else if (s == "false") {
    result = false;
    return true;
  }
  return false;
}

bool ParseBool(const string& str) {
  bool result;
  if (!TryParseBool(str, result))
    throw std::invalid_argument("Could not convert " + str + " into a boolean");
  return result;
}

wstring ConvertStringToWString(const string& str) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(str.c_str());
}

}  // namespace base
