/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <Logger.h>
#include <QDateTime>
#include <QProcessEnvironment>

namespace rmsauth {

String LoggerImpl::getLocalTime(const String& format)
{
    auto now = QDateTime::currentDateTime();
    return std::move(now.toString(QString::fromStdString(format)).toStdString());
}

void Logger::hidden(const String& tag, const String& record)
{
    // read env var
    static QString ev = QProcessEnvironment::systemEnvironment().value("RMS_HIDDEN_LOG", "OFF");
    // if set
    if(QString::compare(ev, "ON") == 0)
    {
        Logger::record("HDN", tag, record);
    }
    // else skip
}

} // namespace rmsauth {
