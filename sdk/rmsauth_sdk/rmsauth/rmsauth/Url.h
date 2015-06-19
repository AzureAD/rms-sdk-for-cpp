/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IURL
#define IURL

#include "types.h"

namespace rmsauth {

class IUrl
{
public:
    virtual void setUrl(const String&) = 0;
    virtual String toString() const = 0;
    virtual String scheme() const = 0;
    virtual String authority() const = 0;
    virtual String host() const = 0;
    virtual String fragment() const = 0;
    virtual String path() const = 0;
    virtual bool isValid() const = 0;
};

class Url : public IUrl
{
public:
    Url();
    Url(const String& url);
    void setUrl(const String& url) override;
    String toString() const override;
    String scheme() const override;
    String authority() const override;
    String host() const override;
    String fragment() const override;
    String path() const override;
    bool isValid() const override;

private:
    ptr<IUrl> pImpl;
};

using UrlPtr = ptr<Url>;

} // namespace rmsauth {

#endif // IURL

