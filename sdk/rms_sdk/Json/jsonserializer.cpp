/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <algorithm>
#include "../ModernAPI/RMSExceptions.h"
#include "../Common/tools.h"
#include "../Common/FrameworkSpecificTypes.h"
#include "../Platform/Json/IJsonObject.h"
#include "../Platform/Json/IJsonArray.h"
#include "../Platform/Json/IJsonParser.h"
#include "jsonserializer.h"

using namespace std;
using namespace rmscore::platform::json;
using namespace rmscore::common;
using namespace rmscore::restclients;

namespace rmscore {
namespace json {
ByteArray JsonSerializer::SerializeUsageRestrictionsRequest(
  const UsageRestrictionsRequest& request)
{
  // convert the PL to base64 encoded string
  ByteArray base64PL(ConvertBytesToBase64(request.pbPublishLicense,
                                          request.cbPublishLicense));

  auto pJsonObject = IJsonObject::Create();

  pJsonObject->SetNamedValue("SerializedPublishingLicense", base64PL);

  return pJsonObject->Stringify();
}

common::ByteArray JsonSerializer::SerializePublishUsingTemplateRequest(
  const PublishUsingTemplateRequest& request)
{
  auto pJson = IJsonObject::Create();

  pJson->SetNamedBool("PreferDeprecatedAlgorithms",
                      request.bPreferDeprecatedAlgorithms);
  pJson->SetNamedBool("AllowAuditedExtraction",
                      request.bAllowAuditedExtraction);
  pJson->SetNamedString("TemplateId", request.templateId);

  if (!request.signedApplicationData.empty())
  {
    auto pSignedApplicationDataJson = IJsonObject::Create();

    for_each(
      begin(request.signedApplicationData),
      end(request.signedApplicationData),
      [ = ](const pair<string, string>& appData)
        {
          // pSignedApplicationDataJson->SetNamedString(appData.name,
          // appData.value);
          pSignedApplicationDataJson->SetNamedString(appData.first,
                                                     appData.second);
        });

    pJson->SetNamedObject("SignedApplicationData", *pSignedApplicationDataJson);
  }

  return pJson->Stringify();
}

common::ByteArray JsonSerializer::SerializePublishCustomRequest(
  const PublishCustomRequest& request)
{
  auto pJson = IJsonObject::Create();

  // Request
  // {
  //	"PreferDeprecatedAlgorithms": ...,
  //	"AllowAuditedExtraction": ...,
  //	"ReferralInfo":...,
  //	"SignedApplicationData": {
  //          "SignedAppDataName1": "EncryptedAppDataValue1"
  //          "SignedAppDataName2": "EncryptedAppDataValue2"
  //          ...
  //      },
  //    "Policy" : {
  //          "Descriptors": [{
  //              "Name": "Protected by Microsoft AADRM service",
  //              "Description" : "This email is protected with Microsoft AADRM
  // service. Your actions may be audited",
  //              "Language" : "en-us"
  //            }],
  //	      ...
  //      }
  // }

  pJson->SetNamedBool("PreferDeprecatedAlgorithms",
                      request.bPreferDeprecatedAlgorithms);
  pJson->SetNamedBool("AllowAuditedExtraction",
                      request.bAllowAuditedExtraction);

  // Add ReferralInfo only when referrer is set
  if (request.wsReferralInfo.length() > 0)
  {
    pJson->SetNamedString("ReferralInfo", request.wsReferralInfo);
  }

  if (!request.signedApplicationData.empty())
  {
    auto pSignedApplicationDataJson = IJsonObject::Create();

    for_each(
      begin(request.signedApplicationData),
      end(request.signedApplicationData),
      [ = ](const pair<string, string>& appData)
        {
          pSignedApplicationDataJson->SetNamedString(appData.first,
                                                     appData.second);
        });

    pJson->SetNamedObject("SignedApplicationData", *pSignedApplicationDataJson);
  }

  auto pPolicyJson = IJsonObject::Create();

  // Add Descriptors
  if (!request.name.empty() &&
      !request.description.empty() &&
      !request.language.empty()) {
    auto pDescriptorObjectArray = IJsonArray::Create();
    auto pDescriptorObject      = IJsonObject::Create();
    pDescriptorObject->SetNamedString("Name",        request.name);
    pDescriptorObject->SetNamedString("Description", request.description);
    pDescriptorObject->SetNamedString("Language",    request.language);
    pDescriptorObjectArray->Append(*pDescriptorObject);
    pPolicyJson->SetNamedArray("Descriptors", *pDescriptorObjectArray);
  }

  // the user rights list json in the policy looks something like this:
  // "Policy": {
  //      ...
  //      "UserRights": [
  //          {
  //              "Users": [user11, user12, ...]
  //              "Rights": [right11, right12, ...]
  //          },
  //          {
  //              "Users": [user21, user22, ...]
  //              "Rights": [right21, right22, ...]
  //          },
  //          ...
  //      ],
  //      "UserRoles":[
  //          {
  //              "Users": [user1, user2, ...],
  //              "Roles": [role1, role2, ...]
  //          },
  //          ...
  //      ],
  //      ...
  // }


  AddUserRightsOrRolesInCustomRequest(pPolicyJson.get(), request);

  pPolicyJson->SetNamedBool("allowOfflineAccess", request.bAllowOfflineAccess);

  // old version support
  pPolicyJson->SetNamedNumber("IntervalTimeInDays",
                              request.bAllowOfflineAccess ? 30 : 0);

  if (std::chrono::system_clock::to_time_t(request.ftLicenseValidUntil) > 0)
  {
    common::DateTime dt = common::DateTime::fromTime_t(
      std::chrono::system_clock::to_time_t(request.ftLicenseValidUntil));
        pPolicyJson->SetNamedString("LicenseValidUntil", dt.toUTC().toString(
                                  Qt::ISODate).toStdString());
  }

  // the appdata should look like this:
  // "Policy": {
  //      ...
  //      "EncryptedApplicationData": {
  //          "EncryptedAppDataName1": "EncryptedAppDataValue1"
  //          "EncryptedAppDataName2": "EncryptedAppDataValue2"
  //          ...
  //      },
  //      ...
  // }

  if (!request.encryptedApplicationData.empty())
  {
    auto pEncryptedApplicationDataJson = IJsonObject::Create();

    for_each(
      begin(request.encryptedApplicationData),
      end(request.encryptedApplicationData),
      [ = ](const pair<string, string>appData)
        {
          pEncryptedApplicationDataJson->SetNamedString(appData.first.
                                                        c_str(),
                                                        appData.second.
                                                        c_str());
        });

    pPolicyJson->SetNamedObject("EncryptedApplicationData",
                                *pEncryptedApplicationDataJson);
  }

  pJson->SetNamedObject("Policy", *pPolicyJson);
  return pJson->Stringify();
}

void JsonSerializer::AddUserRightsOrRolesInCustomRequest(
  IJsonObject                *pPolicyJson,
  const PublishCustomRequest& request)
{
  auto userName     = "Users";
  auto UserProperty = request.userRightsList.size() !=
                      0 ? "Rights" : "Roles";
  auto jsonArrayName = request.userRightsList.size() !=
                       0 ? "UserRights" : "UserRoles";
  auto pUserRightsOrRolesListJson = IJsonArray::Create();

  if (request.userRightsList.size() != 0)
  {
    for_each(
      begin(request.userRightsList),
      end(request.userRightsList),
      [ = ](const UserRightsRequest& userRights)
        {
          auto pUsersJson = IJsonArray::Create();

          for_each(
            begin(userRights.users),
            end(userRights.users),
            [ = ](const string& user)
          {
            pUsersJson->Append(user);
          });

          auto pRightsJson = IJsonArray::Create();

          for_each(
            begin(userRights.rights),
            end(userRights.rights),
            [ = ](const string& right)
          {
            pRightsJson->Append(right);
          });

          auto pUserRightsJson = IJsonObject::Create();

          pUserRightsJson->SetNamedArray(userName,     *pUsersJson);
          pUserRightsJson->SetNamedArray(UserProperty, *pRightsJson);

          pUserRightsOrRolesListJson->Append(*pUserRightsJson);
        });
  }
  else
  {
    for_each(
      begin(request.userRolesList),
      end(request.userRolesList),
      [ = ](const UserRolesRequest& useroles)
        {
          auto pUsersJson = IJsonArray::Create();

          for_each(
            begin(useroles.users),
            end(useroles.users),
            [ = ](const string& user)
          {
            pUsersJson->Append(user);
          });

          auto pRolesJson = IJsonArray::Create();

          for_each(
            begin(useroles.roles),
            end(useroles.roles),
            [ = ](const string& role)
          {
            pRolesJson->Append(role);
          });

          auto pUserRolesJson = IJsonObject::Create();

          pUserRolesJson->SetNamedArray(userName,     *pUsersJson);
          pUserRolesJson->SetNamedArray(UserProperty, *pRolesJson);

          pUserRightsOrRolesListJson->Append(*pUserRolesJson);
        });
  }

  pPolicyJson->SetNamedArray(jsonArrayName, *pUserRightsOrRolesListJson);
}

UsageRestrictionsResponse JsonSerializer::DeserializeUsageRestrictionsResponse(
  common::ByteArray& sResponse)
{
  shared_ptr<IJsonParser> pJsonParser = IJsonParser::Create();

  // parse the JSON
  shared_ptr<IJsonObject> pJsonResponse = pJsonParser->Parse(sResponse);

  UsageRestrictionsResponse response;

  if (pJsonResponse == nullptr) return response;

  response.accessStatus = pJsonResponse->GetNamedString("AccessStatus");
  response.id           = pJsonResponse->GetNamedString("Id");
  response.name         = pJsonResponse->GetNamedString("Name");
  response.description  = pJsonResponse->GetNamedString("Description");
  response.referrer     =
    ProcessReferrerResponse(pJsonResponse->GetNamedString("Referrer"));
  response.owner     = pJsonResponse->GetNamedString("Owner");
  response.issuedTo  = pJsonResponse->GetNamedString("IssuedTo");
  response.contentId = pJsonResponse->GetNamedString("ContentId");

  // Key
  if (pJsonResponse->HasName("Key") && !pJsonResponse->IsNull("Key"))
  {
    shared_ptr<IJsonObject> pJsonKey = pJsonResponse->GetNamedObject("Key");
    response.key.algorithm  = pJsonKey->GetNamedString("Algorithm");
    response.key.cipherMode = pJsonKey->GetNamedString("CipherMode");
    response.key.value      = pJsonKey->GetNamedValue("Value");

    // BUG 101481: There is a bug in PROD where the AccessStatus field is
    // missing. When the fix reaches production, the below
    // statement should be removed.
    if (response.accessStatus.empty())
    {
      response.accessStatus = "AccessGranted";
    }
  }
  else
  {
    // BUG 101481: There is a bug in PROD where the AccessStatus field is
    // missing. When the fix reaches production, the below
    // statement should be removed.
    if (response.accessStatus.empty())
    {
      response.accessStatus = "AccessDenied";
    }
  }

  // Rights
  response.rights = pJsonResponse->GetNamedStringArray("Rights");

  if (pJsonResponse->HasName("Roles") && !pJsonResponse->IsNull("Roles"))
  {
    response.roles = pJsonResponse->GetNamedStringArray("Roles");
  }

  // expiry times
  response.contentValidUntil = pJsonResponse->GetNamedString("ContentValidUntil");
  response.licenseValidUntil = pJsonResponse->GetNamedString("LicenseValidUntil");

  response.bFromTemplate = pJsonResponse->GetNamedBool("FromTemplate");

  // parse expiry times
  if (!response.contentValidUntil.empty())
  {
    // add Z at end - because Content Valid in UTC
    if(response.contentValidUntil.end()[-1] != 'Z') {
        response.contentValidUntil += 'Z';
    }
    auto tmp = common::DateTime::fromString(
      QString::fromStdString(response.contentValidUntil), Qt::ISODate);
    response.ftContentValidUntil = std::chrono::system_clock::from_time_t(
      tmp.toLocalTime().toTime_t());
  }
  else
  {
    response.ftContentValidUntil = std::chrono::system_clock::from_time_t(0);
  }

  if (!response.licenseValidUntil.empty())
  {
    auto tmp =
      common::DateTime::fromString(
        QString::fromStdString(response.licenseValidUntil), Qt::ISODate);
    response.ftLicenseValidUntil = std::chrono::system_clock::from_time_t(
      tmp.toLocalTime().toTime_t());
  }
  else
  {
    response.ftLicenseValidUntil = std::chrono::system_clock::from_time_t(0);
  }

  if (pJsonResponse->HasName("allowOfflineAccess")) {
    response.bAllowOfflineAccess = pJsonResponse->GetNamedBool(
      "allowOfflineAccess", true);
  } else {
    // true by default
    response.bAllowOfflineAccess = true;
  }


  // custom policy response
  if (pJsonResponse->HasName("Policy") && !pJsonResponse->IsNull("Policy"))
  {
    auto pJsonPolicy = pJsonResponse->GetNamedObject("Policy");

    auto intervalTime =
      static_cast<int>(round(pJsonPolicy->GetNamedNumber("IntervalTimeInDays",
                                                         -1.0)));

    if (intervalTime <= 0) response.bAllowOfflineAccess = false;

    response.customPolicy.bAllowAuditedExtraction = pJsonPolicy->GetNamedBool(
      "AllowAuditedExtraction",
      false);

    // UserRights looks something like this
    // "UserRights": [
    //      {"Users": ["user11", "user12", ...], "Rights": ["right11",
    // "right12"], ... }
    //      {"Users": ["user21", "user22", ...], "Rights": ["right21",
    // "right22"], ... }
    //      ...
    // ]
    auto pJsonUserRightsList = pJsonPolicy->GetNamedArray("UserRights");

    for (unsigned iUserRights = 0; iUserRights < pJsonUserRightsList->Size();
         ++iUserRights)
    {
      auto pJsonUserRights = pJsonUserRightsList->GetObjectAt(iUserRights);

      UserRightsResponse userRights;
      userRights.users  = pJsonUserRights->GetNamedStringArray("Users");
      userRights.rights = pJsonUserRights->GetNamedStringArray("Rights");

      response.customPolicy.userRightsList.emplace_back(move(userRights));
    }

    // if UserRoles are present add them too
    if (pJsonPolicy->HasName("UserRoles") && !pJsonPolicy->IsNull("UserRoles"))
    {
      auto pJsonUserRolesList = pJsonPolicy->GetNamedArray("UserRoles");

      for (unsigned iUserRoles = 0; iUserRoles < pJsonUserRolesList->Size();
           ++iUserRoles)
      {
        auto pJsonUserRoles = pJsonUserRolesList->GetObjectAt(iUserRoles);

        UserRolesResponse userRoles;
        userRoles.users = pJsonUserRoles->GetNamedStringArray("Users");
        userRoles.roles = pJsonUserRoles->GetNamedStringArray("Roles");

        response.customPolicy.userRolesList.emplace_back(move(userRoles));
      }
    }

    response.customPolicy.bIsNull = false;
  }
  else
  {
    response.customPolicy.bIsNull = true;
  }

  // signed application data
  if (pJsonResponse->HasName("SignedApplicationData") &&
      !pJsonResponse->IsNull("SignedApplicationData"))
  {
    auto pJsonSignedAppData = pJsonResponse->GetNamedObject(
      "SignedApplicationData");
    response.signedApplicationData = pJsonSignedAppData->ToStringDictionary();
  }

  // encrypted application data
  if (pJsonResponse->HasName("EncryptedApplicationData") &&
      !pJsonResponse->IsNull("EncryptedApplicationData"))
  {
    auto pJsonEncryptedAppData = pJsonResponse->GetNamedObject(
      "EncryptedApplicationData");
    response.encryptedApplicationData =
      pJsonEncryptedAppData->ToStringDictionary();
  }

  return response;
}

ServerErrorResponse JsonSerializer::DeserializeErrorResponse(
  ByteArray& sResponse)
{
  shared_ptr<IJsonParser> pJsonParser = IJsonParser::Create();

  // parse the JSON
  shared_ptr<IJsonObject> pJsonResponse = pJsonParser->Parse(sResponse);

  ServerErrorResponse response;

  // code
  if (pJsonResponse->HasName("Code") && !pJsonResponse->IsNull("Code"))
  {
    response.code = pJsonResponse->GetNamedString("Code");
  }

  // messsage
  if (pJsonResponse->HasName("Message") && !pJsonResponse->IsNull("Message"))
  {
    response.message = pJsonResponse->GetNamedString("Message");
  }

  return response;
}

TemplateListResponse JsonSerializer::DeserializeTemplateListResponse(
  ByteArray& sResponse)
{
  auto pJsonParser = IJsonParser::Create();

  // template list should be an array
  auto pJsonArray = pJsonParser->ParseArray(sResponse);

  TemplateListResponse response;

  for (uint32_t index = 0; index < pJsonArray->Size(); ++index)
  {
    auto pTemplateJson = pJsonArray->GetObjectAt(index);

    auto atemplate = TemplateResponse {
      pTemplateJson->GetNamedString("Id"),
      pTemplateJson->GetNamedString("Name"),
      pTemplateJson->GetNamedString("Description")
    };

    if (atemplate.id.empty())
    {
      throw exceptions::RMSInvalidArgumentException("empty atemplate.id");
    }

    if (atemplate.name.empty())
    {
      throw exceptions::RMSInvalidArgumentException("empty atemplate.name");
    }

    if (atemplate.description.empty())
    {
      throw exceptions::RMSInvalidArgumentException("empty atemplate.description");
    }

    response.templates.emplace_back(move(atemplate));
  }

  return response;
}

PublishResponse JsonSerializer::DeserializePublishResponse(ByteArray& sResponse)
{
  auto pJsonParser = IJsonParser::Create();

  // parse the JSON
  auto pJson = pJsonParser->Parse(sResponse);

  PublishResponse response;

  response.serializedLicense =
    ConvertBase64ToBytes(pJson->GetNamedValue("SerializedPublishingLicense"));
  response.id          = pJson->GetNamedString("Id");
  response.name        = pJson->GetNamedString("Name");
  response.description = pJson->GetNamedString("Description");
  response.referrer    = pJson->GetNamedString("Referrer");
  response.owner       = pJson->GetNamedString("Owner");
  response.contentId   = pJson->GetNamedString("ContentId");

  auto pJsonKey = pJson->GetNamedObject("Key");

  response.key.algorithm  = pJsonKey->GetNamedString("Algorithm");
  response.key.cipherMode = pJsonKey->GetNamedString("CipherMode");
  response.key.value      = pJsonKey->GetNamedValue("Value");

  // signed application data
  if (pJson->HasName("SignedApplicationData") &&
      !pJson->IsNull("SignedApplicationData"))
  {
    auto pJsonSignedAppData = pJson->GetNamedObject("SignedApplicationData");
    response.signedApplicationData = pJsonSignedAppData->ToStringDictionary();
  }

  // encrypted application data
  if (pJson->HasName("EncryptedApplicationData") &&
      !pJson->IsNull("EncryptedApplicationData"))
  {
    auto pJsonEncryptedAppData =
      pJson->GetNamedObject("EncryptedApplicationData");
    response.encryptedApplicationData =
      pJsonEncryptedAppData->ToStringDictionary();
  }

  if (response.serializedLicense.empty())
  {
    throw exceptions::RMSInvalidArgumentException(
            "empty response.serializedLicense");
  }

  if (response.owner.empty())
  {
    throw exceptions::RMSInvalidArgumentException("empty response.owner");
  }

  if (response.key.value.empty())
  {
    throw exceptions::RMSInvalidArgumentException("empty response.key.value");
  }

  if (response.key.algorithm.empty())
  {
    throw exceptions::RMSInvalidArgumentException("empty response.key.algorithm");
  }

  if (response.key.cipherMode.empty())
  {
    throw exceptions::RMSInvalidArgumentException("empty response.key.cipherMode");
  }
  return response;
}

ServiceDiscoveryListResponse JsonSerializer::DeserializeServiceDiscoveryResponse(
  ByteArray& sResponse)
{
  auto pJsonParser = IJsonParser::Create();

  // service discovery response should be an array
  auto pJsonArray = pJsonParser->ParseArray(sResponse);

  ServiceDiscoveryListResponse response;

  for (uint32_t index = 0; index < pJsonArray->Size(); ++index)
  {
    auto pTemplateJson = pJsonArray->GetObjectAt(index);

    auto endpoint = ServiceDiscoveryResponse {
      pTemplateJson->GetNamedString("Name"),
      pTemplateJson->GetNamedString("Uri")
    };

    if (endpoint.name.empty())
    {
      throw exceptions::RMSInvalidArgumentException("empty response.endpoint.name");
    }

    if (endpoint.uri.empty())
    {
      throw exceptions::RMSInvalidArgumentException("empty response.endpoint.uri");
    }

    response.serviceEndpoints.emplace_back(move(endpoint));
  }

  return response;
}

string JsonSerializer::ProcessReferrerResponse(const string&& referrerResponse)
{
  if (!referrerResponse.empty())
  {
    try
    {
      common::IUri::Create(referrerResponse);
      return move(referrerResponse);
    }
    catch (exceptions::RMSException)
    {
      string withMailTo = "mailto:" + referrerResponse;
      common::IUri::Create(withMailTo);
      return move(withMailTo);
    }
  }
  else
  {
    return referrerResponse;
  }
}

shared_ptr<IJsonSerializer>IJsonSerializer::Create()
{
  return make_shared<JsonSerializer>();
}
} // namespace json
} // namespace rmscore
