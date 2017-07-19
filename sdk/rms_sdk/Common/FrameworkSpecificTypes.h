/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_FRAMEWORKSPESIFIC_H_
#define _RMS_LIB_FRAMEWORKSPESIFIC_H_
#include <stdint.h>
#include <QDateTime>
#include <QDataStream>
#include "CommonTypes.h"

namespace rmscore {
namespace common {
using DateTime = QDateTime;
using DataStream = QDataStream;
using IODevice = QIODevice;
} // namespace common
} // namespace rmscore
#endif // _RMS_LIB_FRAMEWORKSPESIFIC_H_
