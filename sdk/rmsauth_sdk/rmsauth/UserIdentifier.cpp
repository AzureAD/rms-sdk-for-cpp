/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <UserIdentifier.h>

namespace rmsauth {

UserIdentifierPtr UserIdentifier::anyUserSingleton()
{
    static const UserIdentifierPtr uid = std::make_shared<UserIdentifier>("AnyUser", UserIdentifierType::UniqueId);
    return uid;
}

UserIdentifier::UserIdentifier(const String& id, UserIdentifierType type)
{
    if (id.empty())
    {
        throw IllegalArgumentException("id");
    }

    id_ = id;
    type_ = type;
}

UserIdentifierPtr UserIdentifier::anyUser()
{
    return anyUserSingleton();
}

bool UserIdentifier::isAnyUser() const
{
    return (type_ == anyUser()->type() && id_ == anyUser()->id());
}

const String UserIdentifier::uniqueId() const
{
    return (!isAnyUser() && type_ == UserIdentifierType::UniqueId)
        ? id_
        : "";
}

const String UserIdentifier::displayableId() const
{
    return (!isAnyUser() && (type_ == UserIdentifierType::OptionalDisplayableId || type_ == UserIdentifierType::RequiredDisplayableId)
        ? id_
        : "");
}

} // namespace rmsauth {
