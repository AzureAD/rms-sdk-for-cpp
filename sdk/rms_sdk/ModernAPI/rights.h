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
 * @brief Rights supported by all apps.
 */
class DLL_PUBLIC_RMS CommonRights {
public:

public:
 /*!
  * @brief Gets a right that represents content ownership.
  */
  static const std::string Owner()
  {
    return "OWNER";
  }

  /*!
  * @brief Gets a right that allows viewing of protected content.
  */
  static const std::string View()
  {
    return "VIEW";
  }

  /*!
  * @brief Gets a set of all CommonRights.
  */
  static const std::vector<std::string> All()
  {
    return std::vector<std::string>{ CommonRights::View(), CommonRights::Owner() };
  }

  /*!
  * @brief BRP072215 - I think this has been deprecated from the RMS SDK, in general.
  */
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

/*!
* @brief Gets a right that allows the protected content to be edited and saved to the 
* same protected format.
*/
  static const std::string Edit()
  {
    return "EDIT";
  }

  /*!
  * @brief Gets a right that allows content to be extracted from a protected format and 
  * placed in a different AD RMS-protected format.
  */
  static const std::string Export()
  {
    return "EXPORT";
  }

  /*!
  * @brief Gets a right that allows content to be extracted from a protected format and 
  * placed in an unprotected, or a different protected, format. Same value as EmailRights.Extract.
  */
  static const std::string Extract()
  {
    return "EXTRACT";
  }

  /*!
  * @brief Gets a right that allows protected content to be printed. Same value as EmailRights.Print.
  */
  static const std::string Print()
  {
    return "PRINT";
  }

  /*!
  * @brief Gets a right that allows the protected content to be commented on and saved to the same 
  * protected format.
  */
  static const std::string Comment()
  {
    return "COMMENT";
  }

  /*!
  * @brief Gets a collection of all the rights that apply to editable documents.
  */
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

	/*!
	* @brief Gets a right that allows a protected email message to be replied to with a 
	* message that includes a copy of the protected content.
	*/
  static const std::string Reply()
  {
    return "REPLY";
  }

  /*!
  * @brief Gets a right that allows reply-all to a protected email message with a message that 
  * includes a copy of the protected content.
  */
  static const std::string ReplyAll()
  {
    return "REPLYALL";
  }

  /*!
  * @brief Gets a right that allows a protected email message to be forwarded.
  */
  static const std::string Forward()
  {
    return "FORWARD";
  }

  /*!
  * @brief Gets a right that allows content to be extracted from a protected email and placed 
  * in an unprotected, or a different protected, format. Same value as EditableDocumentRights.Extract.
  */
  static const std::string Extract()
  {
    return EditableDocumentRights::Extract();
  }

  /*!
  * @brief Gets a right that allows protected email content to be printed. Same value as EditableDocumentRights.Print.
  */
  static const std::string Print()
  {
    return EditableDocumentRights::Print();
  }

  /*!
  * @brief Gets a list of all the rights that apply to emails.
  */
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
