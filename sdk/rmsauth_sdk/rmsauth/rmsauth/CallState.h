/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CALLSTATE_H
#define CALLSTATE_H
#include "AuthorityType.h"
#include "Guid.h"

namespace rmsauth {

class CallState
{
    Guid correlationId_;
    bool callSync_;
    AuthorityType authorityType_;

public:
    CallState(Guid& correlationId, bool callSync);

    const Guid& correlationId() const           { return correlationId_;}   // getter
    void correlationId(const Guid& val)         { correlationId_ = val;}   // setter

    bool callSync() const                 { return callSync_;}
    void callSync(const bool val)               { callSync_ = val;}

    const AuthorityType& authorityType() const   { return authorityType_;}
    void authorityType(const AuthorityType& val) { authorityType_ = val;}
};

using CallStatePtr = ptr<CallState>;

} // namespace rmsauth {

#endif // CALLSTATE_H
