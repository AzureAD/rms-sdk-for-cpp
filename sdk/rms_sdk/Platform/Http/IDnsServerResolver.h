/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_CORE_IDNSSERVERRESOLVER_H_
#define _RMS_CORE_IDNSSERVERRESOLVER_H_

#include <memory>
#include <string>
#include <vector>

#include "../../Common/CommonTypes.h"

namespace rmscore {
namespace platform {
namespace http {
class IDnsServerResolver {
public:
    virtual std::string lookup(const std::string& dnsRequest) = 0;

public:
    static std::shared_ptr<IDnsServerResolver> Create();
};
} // namespace http
} // namespace platform
} // namespace rmscore
#endif // _RMS_CORE_IDNSSERVERRESOLVER_H_
