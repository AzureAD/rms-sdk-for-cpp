/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IJSONARRAY
#define IJSONARRAY

#include <memory>
#include <string>
#include <stdint.h>

#include "../../Common/FrameworkSpecificTypes.h"

namespace rmscore { namespace platform { namespace json {

class IJsonObject;

class IJsonArray
{
public:
    virtual uint32_t Size() = 0;

    virtual const std::string GetStringAt(uint32_t index) = 0;
    virtual std::shared_ptr<IJsonObject> GetObjectAt(uint32_t index) = 0;

    virtual void Clear() = 0;

    virtual void Append(const IJsonObject& jsonObject) = 0;
    virtual void Append(const std::string& name) = 0;

    virtual common::ByteArray Stringify() = 0;

public:
    static std::shared_ptr<IJsonArray> Create();
};

}}} // namespace rmscore { namespace platform { namespace json {


#endif // IJSONARRAY

