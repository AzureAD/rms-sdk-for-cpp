/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <utils.h>
#include <QCryptographicHash>

namespace rmsauth {

QUrlQuery Utils::toQUrlQuery(const StringMap& params)
{
    QUrlQuery urlQuery = QUrlQuery();
    for (auto& pair : params)
    {
        urlQuery.addQueryItem(pair.first.data(), pair.second.data());
    }
    return urlQuery;
};

String HashUtils::createSha256Hash(const String& token)
{
    QCryptographicHash h(QCryptographicHash::Sha256);
    h.addData(token.data());
    auto res = h.result();
    return String(res.begin(), res.end());
}

} // namespace rmsauth {
