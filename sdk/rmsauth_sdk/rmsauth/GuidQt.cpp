/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "GuidQt.h"

namespace rmsauth {

Guid::Guid()
{
    this->pImpl = std::make_shared<GuidQt>();
}

Guid::Guid(const String& str)
{
    this->pImpl = std::make_shared<GuidQt>(QString::fromStdString(str));
}

Guid::Guid(ptr<IGuid> guid)
{
    this->pImpl = guid;
}

Guid Guid::newGuid()
{
    auto pImpl = std::make_shared<GuidQt>(QUuid::createUuid());
    return Guid(pImpl);
}

GuidQt::GuidQt()
{
}

GuidQt::GuidQt(const QUuid& uuid):uuid_(uuid)
{
}
GuidQt::GuidQt(const QString& str):uuid_(str)
{
}

String GuidQt::toString() const
{
    return this->uuid_.toString().toStdString();
}

 bool GuidQt::empty() const
 {
     return this->uuid_.isNull();
 }

} // namespace rmsauth {
