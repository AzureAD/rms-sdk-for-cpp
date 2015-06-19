/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <Url.h>

namespace rmsauth {

void Url::setUrl(const String& url)
{
    this->pImpl->setUrl(url);
}
String Url::toString() const
{
    return this->pImpl->toString();
}

String Url::scheme() const
{
    return this->pImpl->scheme();
}
String Url::authority() const
{
    return this->pImpl->authority();
}
String Url::fragment() const
{
    return this->pImpl->fragment();
}
String Url::host() const
{
    return this->pImpl->host();
}
String Url::path() const
{
    return this->pImpl->path();
}
bool Url::isValid() const
{
    return this->pImpl->isValid();
}

} // namespace rmsauth {
