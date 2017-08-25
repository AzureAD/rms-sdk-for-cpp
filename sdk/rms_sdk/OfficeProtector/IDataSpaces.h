/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_OFFICE_PROTECTOR_IDATASPACES_H
#define RMS_SDK_OFFICE_PROTECTOR_IDATASPACES_H

#include <memory>
#include <gsf/gsf.h>
#include "../Common/CommonTypes.h"

using namespace rmscore::common;

namespace rmscore {
namespace officeprotector {

class IDataSpaces
{
public:
    virtual ~IDataSpaces() {}

    virtual void WriteDataSpaces(GsfOutfile* stg,
                                 const ByteArray& publishingLicense) = 0;
    virtual void ReadDataSpaces(GsfInfile* stg,
                                ByteArray& publishingLicense) = 0;

    static std::shared_ptr<IDataSpaces> Create(bool isMetro);
};

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_IDATASPACES_H

