/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_IDATASPACES_H
#define _RMS_LIB_IDATASPACES_H

#include <../Pole/pole.h>
#include "../Common/CommonTypes.h"
using namespace rmscore::pole;
using namespace rmscore::common;
namespace rmscore {
namespace officeprotector {

class IDataSpaces
{
public:
    virtual ~IDataSpaces() {}

    virtual void WriteDataspaces   (Storage *stg, ByteArray publishingLicense) = 0;
    virtual void ReadDataspaces    (Storage *stg, ByteArray &publishingLicense) = 0;

    static std::shared_ptr<IDataSpaces> Create(bool isMetro);
};

} // namespace officeprotector
} //namespace rmscore
#endif // _RMS_LIB_IDATASPACES_H

