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
	*@brief Response should not be cached.
	*/
    RESPONSE_CACHE_NOCACHE = 0x00,

	/*!
	* @brief Response should be cached in memory. Cached response will
  *        be lost when process exits.
	*/
    RESPONSE_CACHE_INMEMORY= 0x01,

	/*!
	*@brief Response should be cached to disk. Cached response will be available
  *       when library is used again.
	*/
    RESPONSE_CACHE_ONDISK  = 0x02,

	/*!
	*@brief Response should be encrypted when written to disk.
	*/
    RESPONSE_CACHE_ENCRYPTED = 0x04,
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_CACHECONTROL_H
