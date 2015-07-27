/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_ROLES_H_
#define _RMS_LIB_ROLES_H_

#include <string>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
/*!
 * @brief Provides implementation for obtaining roles for protecting documents
 */
class DLL_PUBLIC_RMS Roles {
public:

  /*!
   * @brief User will only be able to view the document.
   */
  static const std::string Viewer()
  {
    return "VIEWER";
  }

  /*!
   * @brief User will be able to view and edit the document.
   */
  static const std::string Reviewer()
  {
    return "REVIEWER";
  }

  /*!
   * @brief User will be able to view, edit, copy, and print the document.
   */
  static const std::string Author()
  {
    return "AUTHOR";
  }

  /*!
   * @brief User will have all permissions; view, edit, copy and print.
   */
  static const std::string CoOwner()
  {
    return "COOWNER";
  }
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_ROLES_H_
