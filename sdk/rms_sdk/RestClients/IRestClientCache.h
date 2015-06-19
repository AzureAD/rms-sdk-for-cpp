/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_IRESTCLIENTCACHE_H_
#define _RMS_LIB_IRESTCLIENTCACHE_H_

#include <memory>
#include <vector>
#include <string>
#include "../Common/CommonTypes.h"
#include "ServiceDiscoveryDetails.h"

namespace rmscore {
namespace restclients {

template<typename T>
using _ptr = std::shared_ptr<T>;

class IRestClientCache {
public:
    enum CacheType {
        CACHE_PLAINDATA = 0,
        CACHE_ENCRYPTED
    };

    virtual common::StringArray Lookup(
        const std::string &cacheName,
        const std::string &tag,
        const uint8_t *pbKey, size_t cbKey, bool useHash) = 0;

    virtual void Store(
        const std::string &cacheName,
        const std::string &tag,
        const uint8_t *pbKey, size_t cbKey,
        const std::string &expires,
        const common::ByteArray& strResponse,
        bool useHash) = 0;

    virtual _ptr<ServiceDiscoveryDetails> LookupServiceDiscoveryDetails(
        const std::string &domain) = 0;

    virtual void Store(
        const std::string &domain,
        _ptr<ServiceDiscoveryDetails> serviceDiscoveryDetails,
        const std::string &expires) = 0;

    virtual common::StringArray LookupDnsClientResult(
            const std::string &domain) = 0;

    virtual void StoreDnsClientResult(
            const std::string &domain,
            const std::string &Ttl,
            const std::string &dnsClientResult) = 0;

public:

  static std::shared_ptr<IRestClientCache>Create(CacheType type);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_IRESTCLIENTCACHE_H_
