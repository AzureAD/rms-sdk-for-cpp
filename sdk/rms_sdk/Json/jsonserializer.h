/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_JSONSERIALIZER_H_
#define _RMS_LIB_JSONSERIALIZER_H_

#include "../Platform/Json/IJsonObject.h"
#include "IJsonSerializer.h"

using namespace std;
namespace rmscore {
namespace json {
class JsonSerializer : public IJsonSerializer
{
public:
    virtual vector<uint8_t> SerializeUsageRestrictionsRequest(const restclients::UsageRestrictionsRequest& request, bool bEncode) override;
    virtual vector<uint8_t> SerializePublishUsingTemplateRequest(const restclients::PublishUsingTemplateRequest& request) override;
    virtual vector<uint8_t> SerializePublishCustomRequest(const restclients::PublishCustomRequest& request) override;

    virtual restclients::UsageRestrictionsResponse DeserializeUsageRestrictionsResponse(vector<uint8_t> &vResponse) override;
    virtual restclients::ServerErrorResponse DeserializeErrorResponse(vector<uint8_t> &vResponse) override;
    virtual restclients::TemplateListResponse DeserializeTemplateListResponse(vector<uint8_t> &vResponse) override;
    virtual restclients::PublishResponse DeserializePublishResponse(vector<uint8_t> &vResponse) override;
    virtual restclients::ServiceDiscoveryListResponse DeserializeServiceDiscoveryResponse(vector<uint8_t> &vResponse) override;
    virtual restclients::CertificateResponse DeserializeCertificateResponse(vector<uint8_t> &vResponse) override;

private:
    std::string ProcessReferrerResponse(const std::string&& referrerResponse);
    void AddUserRightsOrRolesInCustomRequest(platform::json::IJsonObject* pPolicyJson, const restclients::PublishCustomRequest& request);
};
} // namespace json
} // namespace rmscore
#endif // _MICROSOFT_PROTECTION_JSONSERIALIZER_H_
