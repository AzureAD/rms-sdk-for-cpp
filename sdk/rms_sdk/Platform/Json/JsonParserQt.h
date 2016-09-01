/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IJSONPARSERQTIMPL
#define IJSONPARSERQTIMPL

#include <QJsonDocument>
#include "IJsonParser.h"

namespace rmscore { namespace platform { namespace json {

class JsonParserQt : public IJsonParser
{
private:
    virtual std::shared_ptr<IJsonObject> Parse(const std::vector<uint8_t>& json) override;
    virtual std::shared_ptr<IJsonArray> ParseArray(const std::vector<uint8_t>& jsonArray) override;

};

}}} // namespace rmscore { namespace platform { namespace json {

#endif // IJSONPARSERQTIMPL

