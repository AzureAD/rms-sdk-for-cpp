/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <QUuid>
#include "tools.h"

#define TIME_CONVERSION_MS_TO_100NS 10000

using namespace std;

namespace rmscore {
namespace common {

tm ConvertStdTimeToGmtTm(time_t time) {
  tm gmTime = { 0 };
  // gmtime is the standard API in C-runtime that is portable.
  // But that function is not thread safe on all platforms since it uses an internal
  // static buffer.
  // The thread safe function is gmtime_s on Windows and gmtime_r in POSIX.
  #ifdef WIN32
  gmtime_s(&gmTime, &time);
  #elif POSIX
  gmtime_r(&time, &gmTime);
  #else
  #error "Compiled on unsupported platform"
  #endif
  return gmTime;
}

string ConvertTmToString(const tm& t, const string& format) {
  char buf[100] = { 0 };
  strftime(buf, sizeof(buf), format.c_str(), &t);
  return buf;
}

string GetCurrentGmtAsString(const string& format) {
  time_t currentTime;
  time(&currentTime);
  tm gmTime = common::ConvertStdTimeToGmtTm(currentTime);
  return ConvertTmToString(gmTime, format);
}

std::time_t GetTimeFromString(const std::string& dateTime, const std::string& format)
{
  // Create a stream which we will use to parse the string,
  // which we provide to constructor of stream to fill the buffer.
  std::stringstream ss{ dateTime };

  // Create a tm object to store the parsed date and time.
  std::tm dt;

  // Now we read from buffer using get_time manipulator
  // and formatting the input appropriately.
  ss >> std::get_time(&dt, format.c_str());

  // Convert the tm structure to time_t value and return.
  return std::mktime(&dt);
}

uint64_t timeToWinFileTime(const time_t& dateTime) {
  // Definition of FILETIME from MSDN:
  // Contains a 64-bit value representing the number of 100-nanosecond intervals
  // since January 1, 1601 (UTC).
  struct tm origin = {0};
  uint64_t seconds;
  origin.tm_hour = 0;   origin.tm_min = 0; origin.tm_sec = 0;
  origin.tm_year = 1601; origin.tm_mon = 1; origin.tm_mday = 1;
  seconds = difftime(dateTime,mktime(&origin));

  // Get offset - note we need 100-nanosecond intervals, hence we multiply by
  // 10000.
  return TIME_CONVERSION_MS_TO_100NS *TIME_CONVERSION_MS_TO_100NS* seconds;
}

ByteArray ConvertBase64ToBytes(const ByteArray& base64str) {
  QByteArray ba;

  ba.append(QByteArray(reinterpret_cast<const char *>(base64str.data()),
                       static_cast<int>(base64str.size())));

  auto convArray = QByteArray::fromBase64(ba);

  return ByteArray(convArray.begin(), convArray.end());
}


ByteArray ConvertBytesToBase64(const ByteArray& bytes) {
  return ConvertBytesToBase64(bytes.data(), bytes.size());
}

ByteArray ConvertBytesToBase64(const void *bytes, const size_t size)
{
  QByteArray ba;

  ba.append(reinterpret_cast<const char *>(bytes), static_cast<int>(size));

  auto convArray = ba.toBase64();

  return ByteArray(convArray.begin(), convArray.end());
}


string GenerateAGuid()
{
  return QUuid::createUuid().toString().toStdString();
//TODO: Linker error resolve for rpc.h and test for posix(uuid.h)
/*
#ifdef WIN32
    UUID uuid;
    UuidCreate ( &uuid );

    unsigned char * str;
    UuidToStringA ( &uuid, &str );

    std::string s( ( char* ) str );

    RpcStringFreeA ( &str );
#else
    uuid_t uuid;
    uuid_generate_random ( uuid );
    char s[37];
    uuid_unparse ( uuid, s );
#endif
    return s;
*/
}
} // namespace common
} // namespace rmscore
