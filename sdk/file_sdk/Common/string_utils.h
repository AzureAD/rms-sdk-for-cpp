#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
using std::string;

namespace mip {
 bool TryParseBool(const string& str, bool& result);
 bool EqualsIgnoreCase(const string& a, const string& b);
} //namespace mip

#endif // STRINGUTILS_H
