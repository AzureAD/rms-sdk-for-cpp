/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef REQUESTPARAMETERS_H
#define REQUESTPARAMETERS_H

#include "types.h"
#include "ClientKey.h"
#include "OAuthConstants.h"

namespace rmsauth {

class RequestParameters
{
public:
    RequestParameters(const String& resource, const ClientKeyPtr clientKey);
    String& operator[](const String& key);
    const String& operator[](const String& key) const;
    void addParam(const String& key, const String& value);
    String toString() const;

    const String& extraQueryParameter() const {return extraQueryParameter_;}
    void extraQueryParameter(const String& val) {extraQueryParameter_ = val;}

    static String uriEncode(const String& value);

private:
    void addClientKey(const ClientKey& clientKey);

private:
    StringMap params_;
    String extraQueryParameter_;
};

} // namespace rmsauth {

#endif // REQUESTPARAMETERS_H
