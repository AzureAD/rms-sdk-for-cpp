/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_ICONSENTCALLBACKIMPL_H_
#define _RMS_LIB_ICONSENTCALLBACKIMPL_H_
#include <vector>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
/*
*@brief Interface for managing user consents. 
*/
class IConsentCallbackImpl {
public:
/*!
 * @brief You, the app developer, will implement this method to prompt the user for their consent.
 * @param email Email of the consentor.
 * @param domain Domain information.
 * @param urls List of URLs that need to provide consent.
 */

  virtual void Consents(const std::string             & email,
                        const std::string             & domain,
                        const std::vector<std::string>& urls) = 0;
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_ICONSENTCALLBACKIMPL_H_
