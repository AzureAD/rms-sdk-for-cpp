/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_COMMONTYPES_H_
#define _RMS_LIB_COMMONTYPES_H_

#include <vector>
#include <stdint.h>
#include "FrameworkSpecificTypes.h"
#include "../Platform/Http/IUri.h"

namespace rmscore {
namespace common {
template<typename T>
using TIterable   = std::vector<T>;
using ByteArray   = TIterable<uint8_t>;
using CharArray   = TIterable<char>;
using StringArray = TIterable<std::string>;
using IUri        = rmscore::platform::http::IUri;
using UrlArray    = TIterable<std::shared_ptr<IUri>>;

#ifdef _MSC_VER
# define snprintf _snprintf
#else
# define _strcmpi strcasecmp
# define _stricmp strcasecmp
# define _strnicmp strncasecmp
#endif // _MSC_VER
} // namespace common
} // namespace rmscore

#endif // _RMS_LIB_COMMONTYPES_H_
