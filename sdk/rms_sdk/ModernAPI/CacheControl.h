/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_CACHECONTROL_H
#define _RMS_LIB_CACHECONTROL_H

namespace rmscore {
namespace modernapi {
enum ResponseCacheFlags {
    RESPONSE_CACHE_NOCACHE = 0x00,
    RESPONSE_CACHE_INMEMORY= 0x01,
    RESPONSE_CACHE_ONDISK  = 0x02,
    RESPONSE_CACHE_CRYPTED = 0x04,
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_CACHECONTROL_H

