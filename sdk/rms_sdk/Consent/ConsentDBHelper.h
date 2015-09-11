/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#pragma once
#ifndef _RMS_LIB_CONSENTDBHELPER_H_
# define _RMS_LIB_CONSENTDBHELPER_H_

# include <string>
# include <vector>
# include <fstream>
# include <iostream>
# include <memory>
# include <QDebug>
# include <QStandardPaths>

namespace rmscore {
namespace consent {
struct UserDomain
{
  std::string userId;
  std::string domain;
};

// DB helper class
// Provides a wrapper for accessing data from file
class ConsentDBHelper {
public:

  static ConsentDBHelper& GetInstance();

  bool                    Initialize(
    const std::string& path = (QStandardPaths::writableLocation(
                                 QStandardPaths::HomeLocation) +
                               "/.ms-ad/").toStdString());
  void AddApprovedServiceDomain(const std::string& userId,
                                const std::string& domain);
  void AddDocumentTrackingConsent(const std::string& userId,
                                  const std::string& domain);
  bool IsApprovedServiceDomainPresent(
    const std::string& userId,
    const std::string& domain);
  bool IsDocumentTrackingConsentnPresent(
    const std::string& userId,
    const std::string& domain);
  void ClearAllEntriesFromDataBase();

private:

  bool                   ReadFileContent(std::fstream           & stream,
                                         std::vector<UserDomain>& content);
  bool                   IsConsentPresentCommon(const std::string      & userId,
                                                const std::string      & domain,
                                                std::vector<UserDomain>& cache);
  std::vector<UserDomain>GetApprovedDomainsForConsent(
    const std::string      & userId,
    const std::string      & domain,
    std::vector<UserDomain>& cache);
  std::vector<std::string>GetPossibleDomainNames(const std::string& domain);
  UserDomain              AddConsentCommon(const std::string& userId,
                                           const std::string& domain,
                                           std::fstream     & stream);
  void                    AddLine(const std::string& line,
                                  std::fstream     & stream);

  const std::string m_serviceUrlDBFileName = std::string(
    "approvedserviceconsents.db");
  const std::string m_documentTrackingDBFileName = std::string(
    "approveddoctrackingconsents.db");
  const std::string m_genericUserName = std::string("GENERIC_USER");
  static const std::string logFileName;
  static const std::string logEnableAlways;
  static const std::string logEnableNever;
  static const std::string logEnableNow;

  bool m_init   = false;
  bool m_failed = false;
  std::fstream m_serviceUrlFile;
  std::fstream m_docTrackingFile;
  std::vector<UserDomain> m_serviceUrlCache;
  std::vector<UserDomain> m_docTrackingCache;
};
} // namespace consent
} // namespace rmscore
#endif // _RMS_LIB_CONSENTDBHELPER_H_
