/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_OFFICE_PROTECTOR_IDATASPACES_H
#define RMS_SDK_OFFICE_PROTECTOR_IDATASPACES_H

#include "../Common/CommonTypes.h"
#include "pole.h"

using namespace rmscore::common;

namespace rmscore {
namespace officeprotector {

class IDataSpaces
{
public:
    virtual ~IDataSpaces() {}

    virtual void WriteDataspaces(const std::shared_ptr<pole::Storage>& stg,
                                 const ByteArray& publishing_license) = 0;
    virtual void ReadDataspaces(const std::shared_ptr<pole::Storage>& stg,
                                ByteArray& publishing_license) = 0;

    static std::shared_ptr<IDataSpaces> Create(bool isMetro);
};

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_IDATASPACES_H

