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
    void WriteDataspaces(const std::shared_ptr<pole::Storage>& stg,
                         const ByteArray& publishing_license) override;
    void ReadDataspaces(const std::shared_ptr<pole::Storage>& stg,
                        ByteArray& publishing_license) override;

private:

    void WriteVersion(const std::shared_ptr<pole::Stream>& stm, const std::string& content);
    void ReadAndVerifyVersion(const std::shared_ptr<pole::Stream>& stm, const std::string& contentExpected);
    void WriteDataSpaceMap(const std::shared_ptr<pole::Stream>& stm);
    void WriteDRMDataSpace(const std::shared_ptr<pole::Stream>& stm);
    void WriteTxInfo(const std::shared_ptr<pole::Stream>& stm,
                     const std::string& txClassName,
                     const std::string& featureName);
    void ReadTxInfo(const std::shared_ptr<pole::Stream>& stm,
                    const std::string& txClassName,
                    const std::string& featureName);
    void WritePrimary(const std::shared_ptr<pole::Stream>& stm,  const ByteArray& publishing_license);
    void ReadPrimary(const std::shared_ptr<pole::Stream>& stm, ByteArray& publishing_license);

    bool m_isMetro = true;
    bool m_doesUseDeprecatedAlgorithm;
};

} // namespace officeprotector
} // namespace rmscore

#endif // RMS_SDK_OFFICE_PROTECTOR_DATASPACES_H

