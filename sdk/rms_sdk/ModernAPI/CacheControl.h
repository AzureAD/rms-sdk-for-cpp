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

/*!
 *@brief Flags for server caching management.
 */
enum ResponseCacheFlags {
	/*!
	*@brief Response not cached.
	*/
    RESPONSE_CACHE_NOCACHE = 0x00,

	/*!
	*@brief Response to be cached in memory.
	*/
    RESPONSE_CACHE_INMEMORY= 0x01,

	/*!
	*@brief Response to be cached to disk.
	*/
    RESPONSE_CACHE_ONDISK  = 0x02,

	/*!
	*@brief Response to be encrypted.
	*/
    RESPONSE_CACHE_CRYPTED = 0x04,
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_CACHECONTROL_H

