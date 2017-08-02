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
    std::wstring ws(this->pImpl_->scheme());
    std::string scheme(ws.begin(),ws.end());
    return scheme;
}
const std::string UriQt::GetHost() const
{
    std::wstring ws(this->pImpl_->host());
    std::string host(ws.begin(),ws.end());
    return host;
}
int UriQt::GetPort() const
{
    return this->pImpl_->port();
}
const std::string UriQt::ToString()const
{
  std::wstring ws(this->pImpl_->to_string());
  std::string str(ws.begin(),ws.end());
  return str;
}

}}} // namespace rmscore { namespace platform { namespace http {
#endif



