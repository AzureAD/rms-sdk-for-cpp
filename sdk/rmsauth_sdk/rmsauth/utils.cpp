/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <utils.h>
#include <clocale>
#include <cctype>
#include <algorithm>

namespace rmsauth {

bool StringUtils::startsWith(const String& src, const String& substr)
{
    return src.compare(0, substr.length(), substr) == 0;
}
bool StringUtils::endsWith(const String& src, const String& substr)
{
    return src.compare(src.length()-substr.length(), substr.length(), substr) == 0;
}
int StringUtils::compareIC(const String& src, const String& str)
{
    String srcLow = StringUtils::toLower(src);
    String strLow = StringUtils::toLower(str);
    return srcLow.compare(strLow);
}
bool StringUtils::equalsIC(const String& src, const String& str)
{
    return StringUtils::compareIC(src, str) == 0;
}
String StringUtils::trim(const String& src)
{
    if(src.empty())return src;
    int s = 0;
    while( src[s] == ' '
           || src[s] == '\t'
           || src[s] == '\r'
           || src[s] == '\n')
    {s++;}
    auto e = src.length()-1;
    while( src[e] == ' '
           || src[e] == '\t'
           || src[e] == '\r'
           || src[e] == '\n')
    {e--;}
    return src.substr(s,e-s+1);
}
String StringUtils::removeQuoteInHeaderValue(const String& src)
{
    if(src.empty())return src;
    auto tmp = StringUtils::trim(src);
    int s = 0;
    while( tmp[s] == '\''){s++;}
    while( tmp[s] == '"'){s++;}
    auto e = tmp.length()-1;
    while( tmp[e] == '"'){e--;}
    return tmp.substr(s,e-s+1);
}
String StringUtils::toLower(const String& src)
{
    std::setlocale(LC_ALL, "en_US.UTF-8");
    String result(src);
    for(String::size_type i=0; i<src.length(); ++i)
    {
        result[i] =  std::tolower(src[i]);
    }
    return std::move(result);
}
StringArray StringUtils::split(const String& src, const char delim)
{
    StringArray arr;
    StringStream ss(src);
    String item;
    while (std::getline(ss, item, delim))
    {
       arr.push_back(item);
    }
    if(src[src.length()-1] == delim) arr.push_back("");
    return std::move(arr);
}

String StringUtils::replace(const String& src, const String& from, const String& to)
{
    if(src.empty()) return src;
    String result(src);
    auto pos = src.find(from);
    if(pos == String::npos)
    {
        throw std::invalid_argument("Can't find substring");
    }
    return result.replace(pos, from.length(), to);
}

String StringUtils::replaceAll(const String& src, const String& from, const String& to)
{
    if(src.empty()) return src;

    String result(src);
    size_t offset = 0;
    while(true)
    {
        auto pos = result.find(from, offset);
        if(pos == String::npos)
        {
            return result;
        }
        result = result.replace(pos, from.length(), to);
        offset = pos + to.length();
    }
}

String StringUtils::replaceAll(const String& src, const char form, const char to)
{
    String result(src);
    std::replace(result.begin(), result.end(), form, to);
    return std::move(result);
}

} // namespace rmsauth {
