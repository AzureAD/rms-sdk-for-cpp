/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifdef QTFRAMEWORK
#include "UriQt.h"
namespace rmscore { namespace platform { namespace http {

std::shared_ptr<IUri> IUri::Create(const std::string& uri)
{
    return std::make_shared<UriQt>(uri);
}
const std::string UriQt::GetScheme() const
{
    return this->pImpl_->scheme().toStdString();
}
const std::string UriQt::GetHost() const
{
    return this->pImpl_->host().toStdString();
}
int UriQt::GetPort() const
{
    return this->pImpl_->port();
}
const std::string UriQt::ToString()const
{
    return this->pImpl_->toString().toStdString();
}

}}} // namespace rmscore { namespace platform { namespace http {
#endif


