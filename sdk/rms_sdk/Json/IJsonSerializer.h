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

using namespace std;
class IJsonSerializer
{
public:
    virtual vector<uint8_t> SerializeUsageRestrictionsRequest(const restclients::UsageRestrictionsRequest& request, bool encode) = 0;
    virtual vector<uint8_t> SerializePublishUsingTemplateRequest(const restclients::PublishUsingTemplateRequest& request) = 0;
    virtual vector<uint8_t> SerializePublishCustomRequest(const restclients::PublishCustomRequest& request) = 0;

    virtual restclients::UsageRestrictionsResponse DeserializeUsageRestrictionsResponse(vector<uint8_t> &sResponse) = 0;
    virtual restclients::ServerErrorResponse DeserializeErrorResponse(vector<uint8_t> &sResponse) = 0;
    virtual restclients::TemplateListResponse DeserializeTemplateListResponse(vector<uint8_t> &sResponse) = 0;
    virtual restclients::PublishResponse DeserializePublishResponse(vector<uint8_t> &sResponse) = 0;
    virtual restclients::ServiceDiscoveryListResponse DeserializeServiceDiscoveryResponse(vector<uint8_t> &sResponse) = 0;
    virtual restclients::CertificateResponse DeserializeCertificateResponse(vector<uint8_t> &sResponse) = 0;

public:
    static std::shared_ptr<IJsonSerializer> Create();
};
} // namespace json
} // namespace rmscore
#endif //_RMS_LIB_IJSONSERIALIZER_H_
