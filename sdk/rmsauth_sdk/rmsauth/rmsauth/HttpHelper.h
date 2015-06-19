/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef HTTPHELPER_H
#define HTTPHELPER_H

#include "types.h"
#include "RequestParameters.h"
#include "CallState.h"
#include "Entities.h"
#include "rmsauthExport.h"

namespace rmsauth {

class RMSAUTH_EXPORT HttpHelper
{
    static const String& Tag() {static const String tag="HttpHelperQt"; return tag;}

public:
    static TokenResponsePtr sendPostRequestAndDeserializeJsonResponseAsync(const String& uri, const RequestParameters& requestParameters, const CallStatePtr callState);
    // to use trusted CA put certificates
    static bool addCACertificateBase64(const std::vector<uint8_t> &certificate);
    static bool addCACertificateDer(const std::vector<uint8_t> &certificate);

    enum class StatusCode
    {
        Continue = 100,
        SwitchingProtocols = 101,
        OK = 200,
        Created = 201,
        Accepted = 202,
        NonAuthoritativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        MultipleChoices = 300,
        Ambiguous = 300,
        MovedPermanently = 301,
        Moved = 301,
        Found = 302,
        Redirect = 302,
        SeeOther = 303,
        RedirectMethod = 303,
        NotModified = 304,
        UseProxy = 305,
        Unused = 306,
        RedirectKeepVerb = 307,
        TemporaryRedirect = 307,
        BadRequest = 400,
        Unauthorized = 401,
        PaymentRequired = 402,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotAcceptable = 406,
        ProxyAuthenticationRequired = 407,
        RequestTimeout = 408,
        Conflict = 409,
        Gone = 410,
        LengthRequired = 411,
        PreconditionFailed = 412,
        RequestEntityTooLarge = 413,
        RequestUriTooLong = 414,
        UnsupportedMediaType = 415,
        RequestedRangeNotSatisfiable = 416,
        ExpectationFailed = 417,
        UpgradeRequired = 426,
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnavailable = 503,
        GatewayTimeout = 504,
        HttpVersionNotSupported = 505,
    };
};

} // namespace rmsauth {

#endif // HTTPHELPER_H
