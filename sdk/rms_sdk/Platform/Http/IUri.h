/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _IURI_H_
#define _IURI_H_

#include <string>
#include <memory>

namespace rmscore { namespace platform { namespace http {

class IUri {
public:
    virtual const std::string GetScheme() const  = 0;
    virtual const std::string GetHost() const  = 0;
    virtual int GetPort() const  = 0;
    virtual const std::string ToString() const  = 0;
    virtual ~IUri() { }

public:
    static std::shared_ptr<IUri> Create(const std::string& uri);
};

}}} // namespace rmscore { namespace platform { namespace http {

#endif // _IURI_H_

