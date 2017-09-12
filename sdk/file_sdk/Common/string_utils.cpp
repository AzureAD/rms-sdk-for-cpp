#include "string_utils.h"
#include <algorithm>

namespace mip {

bool TryParseBool(const string& str, bool& result) {
  // Perf nit: We do not need a new string that is lower case. We can do
  // a case insensitive comparison with "true" or "false". That has two (nit)
  // perf benefits: another string is not allocated, time complexity is always
  // O(1) since parsing will only do maximum 5 comparisions ("false").
  // TODO: Find a portable way to do case insensitive comparison.
  string s(str);
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
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

bool EqualsIgnoreCase(const string& a, const string& b)
{
    size_t size = a.size();
    if (b.size() != size)
        return false;
    for (size_t i = 0; i < size; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

} //namespace mip
