/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_HTTPHELPER_H_
#define _RMS_LIB_HTTPHELPER_H_

#include <vector>
#include <stdint.h>

#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
/*!
 * @brief Helper class to add additional trusted CA certificates for use
          only with this library.
 */
class DLL_PUBLIC_RMS HttpHelper {
public:

 /*!
  * @brief Adds trusted CA certificates for use only with this library.
           Certificates should be serialized in Base64 format.
  * @param certificate Serialized certificate.
  * @return True if operation succeeded, otherwise false.
  */
  static bool addCACertificateBase64(const std::vector<uint8_t>& certificate);

  /*!
  * @brief Adds trusted CA certificates for use only with this library.
           Certificates should be serialized in DER format.
  * @param certificate Serialized certificate.
  * @return True if operation succeeded, otherwise false.
  */
  static bool addCACertificateDer(const std::vector<uint8_t>& certificate);
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_HTTPHELPER_H_
