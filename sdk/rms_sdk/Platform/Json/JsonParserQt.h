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
    virtual std::shared_ptr<IJsonObject> Parse(const common::ByteArray& json) override;
    virtual std::shared_ptr<IJsonArray> ParseArray(const common::ByteArray& jsonArray) override;

};

}}} // namespace rmscore { namespace platform { namespace json {

#endif // IJSONPARSERQTIMPL

