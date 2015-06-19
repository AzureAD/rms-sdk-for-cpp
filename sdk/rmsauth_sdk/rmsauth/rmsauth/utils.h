/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef UTILS_H
#define UTILS_H

#include "types.h"

namespace rmsauth {

class StringUtils
{
public:
    static bool startsWith(const String& src, const String& substr);
    static bool endsWith(const String& src, const String& substr);
    static int compareIC(const String& src, const String& str);
    static bool equalsIC(const String& src, const String& str);
    static String trim(const String& src);
    static String removeQuoteInHeaderValue(const String& src);
    static String toLower(const String& src);
    static StringArray split(const String& src, const char delim);
    static String replace(const String& src, const String& form, const String& to);
    static String replaceAll(const String& src, const String& form, const String& to);
    static String replaceAll(const String& src, const char form, const char to);
};

class HashUtils
{
public:
    static String createSha256Hash(const String& token);
};

} //namespace rmsauth {

#endif // UTILS_H
