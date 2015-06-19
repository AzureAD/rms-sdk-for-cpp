/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef URLQT_H
#define URLQT_H

#include <QUrl>
#include <Url.h>

namespace rmsauth {

class UrlQt : public IUrl
{
public:
    UrlQt();
    UrlQt(const String& url);
    void setUrl(const String& url) override;
    String toString() const override;
    String scheme() const override;
    String authority() const override;
    String host() const override;
    String fragment() const override;
    String path() const override;
    bool isValid() const override;

private:
    QUrl url_;
};

} // namespace rmsauth {

#endif // URLQT_H
