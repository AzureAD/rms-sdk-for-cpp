/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "DateTimeQt.h"

namespace rmsauth {

DateTime::DateTime()
{
    this->pImpl = std::make_shared<DateTimeQt>();
}
DateTime::DateTime(int64_t sec)
{
    this->pImpl = std::make_shared<DateTimeQt>(sec);
}

DateTimeQt::DateTimeQt()
    : dateTime_(QDateTime::currentDateTimeUtc())
{
}
DateTimeQt::DateTimeQt(int64_t sec)
    : dateTime_(QDateTime::fromTime_t(sec, Qt::UTC))
{
}

void DateTimeQt::addDays(int64_t ndays)
{
    QDateTime dt = this->dateTime_.addDays((qint64)ndays);
    this->dateTime_ = dt;
}
void DateTimeQt::addMSecs(int64_t msecs)
{
     this->dateTime_ = this->dateTime_.addMSecs((qint64)msecs);
}

void DateTimeQt::addMonths(int nmonths)
{
     this->dateTime_ = this->dateTime_.addMonths(nmonths);
}

void DateTimeQt::addSecs(int64_t s)
{
     this->dateTime_ = this->dateTime_.addSecs((qint64)s);
}

void DateTimeQt::addYears(int nyears)
{
     this->dateTime_ = this->dateTime_.addYears(nyears);
}
int64_t DateTimeQt::toTime()
{
     return this->dateTime_.toTime_t();
}

String DateTimeQt::toString(const String& format)
{
    return this->dateTime_.toString(format.c_str()).toStdString();
}

} // namespace rmsauth {
