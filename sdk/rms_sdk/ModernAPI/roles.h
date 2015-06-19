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
/// <summary>
/// This class provides implementation for obtaining roles for protecting documents
/// </summary>
class DLL_PUBLIC_RMS Roles {
public:

  /// <summary>
  /// User will only be able to view the document. They cannot edit, copy, or print it.
  /// </summary>
  static const std::string Viewer()
  {
    return "VIEWER";
  }

  /// <summary>
  /// User will be able to view and edit the document. They cannot copy or print it.
  /// </summary>
  static const std::string Reviewer()
  {
    return "REVIEWER";
  }

  /// <summary>
  /// User will be able to view, edit, copy, and print the document.
  /// </summary>
  static const std::string Author()
  {
    return "AUTHOR";
  }

  /// <summary>
  /// User will have all permissions.
  /// </summary>
  static const std::string CoOwner()
  {
    return "COOWNER";
  }
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_ROLES_H_
