/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_IJSONSERIALIZER_H_
#define _RMS_LIB_IJSONSERIALIZER_H_

#include "../RestClients/RestObjects.h"
#include <memory>
#include <string>
#include <vector>

namespace rmscore {
namespace json {

class IJsonSerializer
{
public:
    virtual common::ByteArray SerializeUsageRestrictionsRequest(const restclients::UsageRestrictionsRequest& request) = 0;
    virtual common::ByteArray SerializePublishUsingTemplateRequest(const restclients::PublishUsingTemplateRequest& request) = 0;
    virtual common::ByteArray SerializePublishCustomRequest(const restclients::PublishCustomRequest& request) = 0;

    virtual restclients::UsageRestrictionsResponse DeserializeUsageRestrictionsResponse(common::ByteArray &sResponse) = 0;
    virtual restclients::ServerErrorResponse DeserializeErrorResponse(common::ByteArray &sResponse) = 0;
    virtual restclients::TemplateListResponse DeserializeTemplateListResponse(common::ByteArray &sResponse) = 0;
    virtual restclients::PublishResponse DeserializePublishResponse(common::ByteArray &sResponse) = 0;
    virtual restclients::ServiceDiscoveryListResponse DeserializeServiceDiscoveryResponse(common::ByteArray &sResponse) = 0;

public:
    static std::shared_ptr<IJsonSerializer> Create();
};
} // namespace json
} // namespace rmscore
#endif //_RMS_LIB_IJSONSERIALIZER_H_
