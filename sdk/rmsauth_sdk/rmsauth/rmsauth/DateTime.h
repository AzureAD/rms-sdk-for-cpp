/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef DATETIME_H
#define DATETIME_H

#include "types.h"

namespace rmsauth {

class IDateTime
{
public:
    virtual void addDays(int64_t ndays) = 0;
    virtual void addMonths(int nmonths) = 0;
    virtual void addYears(int nyears) = 0;
    virtual void addMSecs(int64_t msecs) = 0;
    virtual void addSecs(int64_t s) = 0;
    virtual int64_t toTime() = 0;
    virtual String toString(const String& format) = 0;
};

class DateTime : public IDateTime
{
public:
    DateTime();
    DateTime(int64_t sec);
    void addDays(int64_t ndays) override;
    void addMSecs(int64_t msecs) override;
    void addMonths(int nmonths) override;
    void addSecs(int64_t s) override;
    void addYears(int nyears) override;
    int64_t toTime() override;
    virtual String toString(const String& format) override;
private:
    ptr<IDateTime> pImpl;
};

} //namespace rmsauth {

#endif // DATETIME_H
