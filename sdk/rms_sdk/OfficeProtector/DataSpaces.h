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
#include "pole.h"

using namespace rmscore::common;

namespace rmscore {
namespace officeprotector {

class DataSpaces : public IDataSpaces
{
public:
    DataSpaces(bool isMetro, bool doesUseDeprecatedAlgorithm = true);
    ~DataSpaces();
    void WriteDataspaces(std::shared_ptr<pole::Storage> stg,
                         const ByteArray& publishingLicense) override;
    void ReadDataspaces(std::shared_ptr<pole::Storage> stg,
                        ByteArray& publishingLicense) override;

private:

    void WriteVersion(std::shared_ptr<pole::Stream> stm, const std::string& content);
    void ReadAndVerifyVersion(std::shared_ptr<pole::Stream> stm, const std::string& contentExpected);
    void WriteDataSpaceMap(std::shared_ptr<pole::Stream> stm);
    void WriteDRMDataSpace(std::shared_ptr<pole::Stream> stm);
    void WriteTxInfo(std::shared_ptr<pole::Stream> stm,
                     const std::string& txClassName,
                     const std::string& featureName);
    void ReadTxInfo(std::shared_ptr<pole::Stream> stm,
                    const std::string& txClassName,
                    const std::string& featureName);
    void WritePrimary(std::shared_ptr<pole::Stream> stm,  const ByteArray& publishingLicense);
    void ReadPrimary(std::shared_ptr<pole::Stream> stm, ByteArray& publishingLicense);

    bool m_isMetro = true;
    bool m_doesUseDeprecatedAlgorithm;
};

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_DATASPACES_H

