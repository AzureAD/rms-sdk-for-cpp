/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <Logger.h>
#include <QDateTime>

namespace rmsauth {

String LoggerImpl::getLocalTime(const String& format)
{
    auto now = QDateTime::currentDateTime();
    return std::move(now.toString(QString::fromStdString(format)).toStdString());
}

} // namespace rmsauth {
