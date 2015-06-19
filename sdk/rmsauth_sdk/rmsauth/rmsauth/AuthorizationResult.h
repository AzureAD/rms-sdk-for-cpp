/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef AUTHORIZATIONRESULT_H
#define AUTHORIZATIONRESULT_H

#include "types.h"

namespace rmsauth {

enum class AuthorizationStatus
{
    Failed = -1,
    Success = 1,
};

class AuthorizationResult
{
    static const String& Tag() {static const String tag="AuthorizationResult"; return tag;}

    AuthorizationStatus status_;
    String code_;
    String error_;
    String errorDescription_;

public:
//    AuthorizationResult(const String& code);
//    AuthorizationResult(const String& error, const String& errorDescription);
    AuthorizationResult(const String& code)
        : status_(AuthorizationStatus::Success)
        , code_(code)
    {
    }
    AuthorizationResult(const String& error, const String& errorDescription)
        : status_(AuthorizationStatus::Failed)
        , error_(error)
        , errorDescription_(errorDescription)
    {
    }

    const AuthorizationStatus& status() const { return status_; }
    const String& code() const { return code_; }
    const String& error() const { return error_; }
    const String& errorDescription() const { return errorDescription_; }
};

using AuthorizationResultPtr = ptr<AuthorizationResult>;

} // namespace rmsauth {

#endif // AUTHORIZATIONRESULT_H
