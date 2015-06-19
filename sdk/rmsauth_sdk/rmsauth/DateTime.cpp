/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <DateTime.h>

namespace rmsauth {

void DateTime::addDays(int64_t ndays)
{
    this->pImpl->addDays(ndays);
}
void DateTime::addMSecs(int64_t msecs)
{
    this->pImpl->addMSecs(msecs);
}

void DateTime::addMonths(int nmonths)
{
    this->pImpl->addMonths(nmonths);
}

void DateTime::addSecs(int64_t s)
{
    this->pImpl->addSecs(s);
}

void DateTime::addYears(int nyears)
{
    this->pImpl->addYears(nyears);
}
int64_t DateTime::toTime()
{
    return this->pImpl->toTime();
}
String DateTime::toString(const String& format)
{
    return this->pImpl->toString(format);
}

} // namespace rmsauth {
