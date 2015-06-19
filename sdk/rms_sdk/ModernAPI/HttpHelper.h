/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_HTTPHELPER_H
#define _RMS_LIB_HTTPHELPER_H

#include <vector>
#include <stdint.h>

#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
class DLL_PUBLIC_RMS HttpHelper {
public:

  // to use trusted CA put certificates
  static bool addCACertificateBase64(const std::vector<uint8_t>& certificate);
  static bool addCACertificateDer(const std::vector<uint8_t>& certificate);
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_HTTPHELPER_H
