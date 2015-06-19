/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "UrlQt.h"
#include <Url.h>

namespace rmsauth {

Url::Url() : pImpl(std::make_shared<UrlQt>())
{
}

Url::Url(const String& url) : pImpl(std::make_shared<UrlQt>(url.data()))
{
}

UrlQt::UrlQt()
{
}

UrlQt::UrlQt(const String& url) : url_(url.data())
{
}

void UrlQt::setUrl(const String& url)
{
    this->url_.setUrl(url.data());
}

String UrlQt::toString() const
{
    return this->url_.toString().toStdString();
}

String UrlQt::scheme() const
{
    return this->url_.scheme().toStdString();
}
String UrlQt::authority() const
{
    return this->url_.authority().toStdString();
}
String UrlQt::host() const
{
    return this->url_.host().toStdString();
}
String UrlQt::fragment() const
{
    return this->url_.fragment().toStdString();
}
String UrlQt::path() const
{
    return this->url_.path().toStdString();
}
bool UrlQt::isValid() const
{
    return this->url_.isValid();
}

} // namespace rmsauth {
