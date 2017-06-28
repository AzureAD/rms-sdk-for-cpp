/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_OFFICE_PROTECTOR_DATASPACES_H
#define RMS_SDK_OFFICE_PROTECTOR_DATASPACES_H

#include "IDataSpaces.h"
#include "../Common/CommonTypes.h"

using namespace rmscore::common;

namespace rmscore {
namespace officeprotector {

class DataSpaces : public IDataSpaces
{
public:
    DataSpaces(bool isMetro, bool doesUseDeprecatedAlgorithm = true);
    ~DataSpaces();
    void WriteDataspaces(GsfOutfile* stg,
                         const ByteArray& publishingLicense) override;
    void ReadDataspaces(GsfInfile* stg,
                        ByteArray& publishingLicense) override;

private:

    void WriteVersion(GsfOutput* stm, const std::string& content);
    void ReadAndVerifyVersion(GsfInput* stm, const std::string& contentExpected);
    void WriteDataSpaceMap(GsfOutput* stm);
    void WriteDRMDataSpace(GsfOutput *stm);
    void WriteTxInfo(GsfOutput* stm,
                     const std::string& txClassName,
                     const std::string& featureName);
    void ReadTxInfo(GsfInput* stm,
                    const std::string& txClassName,
                    const std::string& featureName);
    void WritePrimary(GsfOutput* stm,  const ByteArray& publishingLicense);
    void ReadPrimary(GsfInput* stm, ByteArray& publishingLicense);

    bool m_isMetro = true;
    bool m_doesUseDeprecatedAlgorithm;
};

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_DATASPACES_H

