/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef USERIDENTIFIER_H
#define USERIDENTIFIER_H

#include "types.h"
#include "Exceptions.h"

namespace rmsauth {

enum class UserIdentifierType
{
    UniqueId,
    OptionalDisplayableId,
    RequiredDisplayableId
};

class UserIdentifier;
using UserIdentifierPtr = ptr<UserIdentifier>;

class UserIdentifier final
{
    UserIdentifierType type_;
    String id_;

    static UserIdentifierPtr anyUserSingleton();

public:
    UserIdentifier(const String& id, UserIdentifierType type);

    UserIdentifierType type() const { return type_; }

    const String& id() const  { return id_; }

    static UserIdentifierPtr anyUser();

    bool isAnyUser() const;

    const String uniqueId() const;

    const String displayableId() const;
};

} // namespace rmsauth {

#endif // USERIDENTIFIER_H
