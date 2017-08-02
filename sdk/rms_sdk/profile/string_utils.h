/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PROFILE_STRING_UTILS_H_
#define PROFILE_STRING_UTILS_H_

#include <string>

namespace rmscore {

namespace stringutils {

inline char GetDirectorySeparator() {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}

} // namespace stringutils

} // namespace rmscore

#endif  // PROFILE_STRING_UTILS_H_
