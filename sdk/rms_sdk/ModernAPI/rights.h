/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_RIGHTS_H_
#define _RMS_LIB_RIGHTS_H_

#include <string>
#include <vector>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {

/**
 * @brief Rights supported universally
 */
class DLL_PUBLIC_RMS CommonRights {
public:

public:

  static const std::string Owner()
  {
    return "OWNER";
  }

  static const std::string View()
  {
    return "VIEW";
  }

  static const std::vector<std::string> All()
  {
    return std::vector<std::string>{ CommonRights::View(), CommonRights::Owner() };
  }

  static std::string AuditedExtract()
  {
    return "AUDITEDEXTRACT";
  }

private:

  CommonRights() = delete;
  friend class Rights;
};

/**
 * @brief Rights that apply to editable documents.
 */
class DLL_PUBLIC_RMS EditableDocumentRights {
public:

  static const std::string Edit()
  {
    return "EDIT";
  }

  static const std::string Export()
  {
    return "EXPORT";
  }

  static const std::string Extract()
  {
    return "EXTRACT";
  }

  static const std::string Print()
  {
    return "PRINT";
  }

  static const std::string Comment()
  {
    return "COMMENT";
  }

  static const std::vector<std::string> All()
  {
    return std::vector<std::string>{
             CommonRights::View(),
             EditableDocumentRights::Edit(),
             EditableDocumentRights::Extract(),
             EditableDocumentRights::Print(),
             EditableDocumentRights::Comment(),
             CommonRights::Owner()
    };
  }

private:

  EditableDocumentRights() = delete;
};

/**
 * @brief Rights that apply to email.
 */
class DLL_PUBLIC_RMS EmailRights {
public:

  static const std::string Reply()
  {
    return "REPLY";
  }

  static const std::string ReplyAll()
  {
    return "REPLYALL";
  }

  static const std::string Forward()
  {
    return "FORWARD";
  }

  static const std::string Extract()
  {
    return EditableDocumentRights::Extract();
  }

  static const std::string Print()
  {
    return EditableDocumentRights::Print();
  }

  static const std::vector<std::string> All()
  {
    return std::vector<std::string>{
             CommonRights::View(),
             EmailRights::Reply(),
             EmailRights::ReplyAll(),
             EmailRights::Forward(),
             EditableDocumentRights::Extract(),
             EditableDocumentRights::Print(),
             CommonRights::Owner()
    };
  }

private:

  EmailRights() = delete;
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_RIGHTS_H_
