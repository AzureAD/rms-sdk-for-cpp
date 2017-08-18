/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IURIQTIMPL
#define IURIQTIMPL

#include "IUri.h"
#include "QUrl"

namespace rmscore { namespace platform { namespace http {

class UriQt : public IUri
{
public:
    UriQt(const std::string& uri){this->pImpl_= new QUrl(uri.c_str());}
    ~UriQt(){ if(nullptr != this->pImpl_) delete this->pImpl_;}

    virtual const std::string GetScheme() const override;
    virtual const std::string GetHost() const override;
    virtual int GetPort() const override;
    virtual const std::string ToString()const override;

private:
    QUrl* pImpl_;
};

}}} // namespace rmscore { namespace platform { namespace http {

#endif // IURIQTIMPL

