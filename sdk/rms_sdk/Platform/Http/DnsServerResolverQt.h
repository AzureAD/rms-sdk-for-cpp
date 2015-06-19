/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef DNSSERVERRESOLVERQT
#define DNSSERVERRESOLVERQT
#include "IDnsServerResolver.h"

class QUdpSocket;

namespace rmscore {
namespace platform {
namespace http {
class DnsServerResolverQt : public IDnsServerResolver {
public:
    std::string lookup(const std::string& dnsRequest) override;
};
} // namespace http
} // namespace platform
} // namespace rmscore

#endif // DNSSERVERRESOLVERQT

