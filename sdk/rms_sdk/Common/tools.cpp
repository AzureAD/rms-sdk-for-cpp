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
uint64_t timeToWinFileTime(const QDateTime& dateTime) {
  // Definition of FILETIME from MSDN:
  // Contains a 64-bit value representing the number of 100-nanosecond intervals
  // since January 1, 1601 (UTC).
  QDateTime origin(QDate(1601, 1, 1), QTime(0, 0, 0, 0), Qt::UTC);

  // Get offset - note we need 100-nanosecond intervals, hence we multiply by
  // 10000.
  return TIME_CONVERSION_MS_TO_100NS * origin.msecsTo(dateTime);
}

ByteArray ConvertBase64ToBytes(const ByteArray& base64str) {
  QByteArray ba;

  ba.append(QByteArray(reinterpret_cast<const char *>(base64str.data()),
                       static_cast<int>(base64str.size())));

  auto convArray = QByteArray::fromBase64(ba);

  return ByteArray(convArray.begin(), convArray.end());
}

string timeToString(const QDateTime& dateTime) {
  if (!dateTime.isNull())
  {
    //    return dateTime.toString("yyyy-MM-DDThh:mm:ssZ").toStdString();
    return dateTime.toString(Qt::ISODate).toStdString();
  }
  else
  {
    return string();
  }
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
}
} // namespace common
} // namespace rmscore
