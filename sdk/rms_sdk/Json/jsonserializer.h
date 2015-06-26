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

namespace rmscore {
namespace json {
class JsonSerializer : public IJsonSerializer
{
public:
    virtual common::ByteArray SerializeUsageRestrictionsRequest(const restclients::UsageRestrictionsRequest& request) override;
    virtual common::ByteArray SerializePublishUsingTemplateRequest(const restclients::PublishUsingTemplateRequest& request) override;
    virtual common::ByteArray SerializePublishCustomRequest(const restclients::PublishCustomRequest& request) override;

    virtual restclients::UsageRestrictionsResponse DeserializeUsageRestrictionsResponse(common::ByteArray &sResponse) override;
    virtual restclients::ServerErrorResponse DeserializeErrorResponse(common::ByteArray &sResponse) override;
    virtual restclients::TemplateListResponse DeserializeTemplateListResponse(common::ByteArray &sResponse) override;
    virtual restclients::PublishResponse DeserializePublishResponse(common::ByteArray &sResponse) override;
    virtual restclients::ServiceDiscoveryListResponse DeserializeServiceDiscoveryResponse(common::ByteArray &sResponse) override;

private:
    std::string ProcessReferrerResponse(const std::string&& referrerResponse);
    void AddUserRightsOrRolesInCustomRequest(platform::json::IJsonObject* pPolicyJson, const restclients::PublishCustomRequest& request);
};
} // namespace json
} // namespace rmscore
#endif // _MICROSOFT_PROTECTION_JSONSERIALIZER_H_
