/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_RESTOBJECTS_H_
#define _RMS_LIB_RESTOBJECTS_H_

#include <chrono>
#include <string>
#include <vector>
#include <stdint.h>
#include "../Common/CommonTypes.h"
#include "../Platform/Json/IJsonObject.h"

namespace rmscore {
namespace restclients {
struct UsageRestrictionsRequest {
  const uint8_t *pbPublishLicense;
  uint32_t       cbPublishLicense;
};

struct KeyDetailsResponse {
  common::ByteArray value;
  std::string       algorithm;
  std::string       cipherMode;
};

struct UserRightsResponse {
  std::vector<std::string>users;
  std::vector<std::string>rights;
};

struct UserRolesResponse {
  std::vector<std::string>users;
  std::vector<std::string>roles;
};

struct CustomPolicyResponse {
  bool                           bIsNull;
  bool                           bAllowAuditedExtraction;
  std::vector<UserRightsResponse>userRightsList;
  std::vector<UserRolesResponse> userRolesList;
};

struct UsageRestrictionsResponse {
  std::string                                       accessStatus;
  std::string                                       id;
  std::string                                       name;
  std::string                                       description;
  std::string                                       referrer;
  std::string                                       owner;
  KeyDetailsResponse                                key;
  std::vector<std::string>                          rights;
  std::vector<std::string>                          roles;
  std::string                                       issuedTo;
  std::chrono::time_point<std::chrono::system_clock>ftContentValidUntil;
  std::chrono::time_point<std::chrono::system_clock>ftLicenseValidUntil;
  std::string                                       contentValidUntil;
  std::string                                       licenseValidUntil;
  bool                                              bAllowOfflineAccess;
  bool                                              bFromTemplate;
  std::string                                       contentId;
  CustomPolicyResponse                              customPolicy;
  modernapi::AppDataHashMap                         signedApplicationData;
  modernapi::AppDataHashMap                         encryptedApplicationData;
};

struct ServerErrorResponse {
  std::string code;
  std::string message;
};

struct TemplateResponse {
  std::string id;
  std::string name;
  std::string description;
};

struct TemplateListResponse {
  std::vector<TemplateResponse>templates;
};

struct ServiceDiscoveryResponse {
  std::string name;
  std::string uri;
};

struct ServiceDiscoveryListResponse {
  std::vector<ServiceDiscoveryResponse>serviceEndpoints;
};

struct PublishUsingTemplateRequest {
  bool                      bPreferDeprecatedAlgorithms;
  bool                      bAllowAuditedExtraction;
  std::string               templateId;
  modernapi::AppDataHashMap signedApplicationData;
};

struct UserRightsRequest {
  std::vector<std::string>users;
  std::vector<std::string>rights;
};

struct UserRolesRequest {
  std::vector<std::string>users;
  std::vector<std::string>roles;
};

struct ApplicationDataRequest {
  std::string name;
  std::string value;
};

struct PublishCustomRequest {
  PublishCustomRequest(bool bPreferDeprecatedAlgorithms,
                       bool bAllowAuditedExtraction)
    : bPreferDeprecatedAlgorithms(bPreferDeprecatedAlgorithms)
    , bAllowAuditedExtraction(bAllowAuditedExtraction) {}

  bool        bPreferDeprecatedAlgorithms;
  bool        bAllowAuditedExtraction;
  std::string wsReferralInfo;

  // Descriptors
  std::string                                       name;
  std::string                                       description;
  std::string                                       language;
  std::vector<UserRightsRequest>                    userRightsList;
  std::vector<UserRolesRequest>                     userRolesList;
  bool                                              bAllowOfflineAccess;
  std::chrono::time_point<std::chrono::system_clock>ftLicenseValidUntil;
  modernapi::AppDataHashMap                         signedApplicationData;
  modernapi::AppDataHashMap                         encryptedApplicationData;
};

struct PublishResponse {
  common::ByteArray         serializedLicense;
  std::string               id;
  std::string               name;
  std::string               description;
  std::string               referrer;
  std::string               owner;
  KeyDetailsResponse        key;
  std::string               contentId;
  modernapi::AppDataHashMap signedApplicationData;
  modernapi::AppDataHashMap encryptedApplicationData;
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_RESTOBJECTS_H_
