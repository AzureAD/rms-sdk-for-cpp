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
/*!
 * @brief Helper class provides methods for ...
 */
class DLL_PUBLIC_RMS HttpHelper {
public:

 /*!
  * @brief To use trusted CA put certificates - Base64
  * @param certificate
  * @return True or False
  */
  static bool addCACertificateBase64(const std::vector<uint8_t>& certificate);

  /*!
  * @brief To use trusted CA put certificates - DER
  * @param certificate
  * @return True or False
  */
  static bool addCACertificateDer(const std::vector<uint8_t>& certificate);
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_HTTPHELPER_H
