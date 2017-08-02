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
#include "../../Common/tools.h"
#include <cpprest/base_uri.h>
#include <cpprest/asyncrt_utils.h>

namespace rmscore { namespace platform { namespace http {

class UriQt : public IUri
{
public:
    UriQt(const std::string& uri){
        web::uri Uri=web::uri::uri(utility::conversions::to_string_t(uri));
        this->pImpl_= &Uri;
    }

    ~UriQt(){if(nullptr != this->pImpl_) delete this->pImpl_;}

    virtual const std::string GetScheme() const override;
    virtual const std::string GetHost() const override;
    virtual int GetPort() const override;
    virtual const std::string ToString()const override;

private:
    web::uri* pImpl_;
};

}}} // namespace rmscore { namespace platform { namespace http {

#endif // IURIQTIMPL


