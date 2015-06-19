/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <RequestParameters.h>
#include <QUrl>

namespace rmsauth {

String RequestParameters::uriEncode(const String& value)
{
    auto encodedData = QUrl::toPercentEncoding(QString::fromStdString(value));
    return String(encodedData.data(), encodedData.size());
}

} // namespace rmsauth {
