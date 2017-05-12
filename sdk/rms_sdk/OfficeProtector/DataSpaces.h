/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_DATASPACES_H
#define _RMS_LIB_DATASPACES_H

#include <../Pole/pole.h>
#include "IDataSpaces.h"
#include "../Common/CommonTypes.h"
using namespace rmscore::pole;
using namespace rmscore::common;
namespace rmscore {
namespace officeprotector {

//DataSpaceMap Header
typedef struct
{
    uint32_t headerLen;
    uint32_t entryCount;
} DataSpaceMapHeader;

//DataSpaceMap Entry Header
typedef struct
{
    uint32_t entryLen;
    uint32_t componentCount;
} DataSpaceMapEntryHeader;

//DRM Transform Header
typedef struct
{
    uint32_t headerLen;
    uint32_t txCount;
} DRMTransformHeader;

//DRM Transform Info
typedef struct
{
    uint32_t headerLen;
    uint32_t txClassType;
} DRMTransformInfo;

class DataSpaces : public IDataSpaces
{
public:
    DataSpaces(bool);

public:
    virtual ~DataSpaces();
    virtual void WriteDataspaces   (Storage *stg, ByteArray publishingLicense) override;
    virtual void ReadDataspaces    (Storage *stg, ByteArray &publishingLicense) override;


private:

    void WriteVersion      (Stream *stm, std::string content);
    void ReadVersion       (Stream *stm, std::string contentExpected);
    void WriteDataSpaceMap (Stream *stm);
    void WriteDRMDataSpace (Stream *stm);
    void WriteTxInfo       (Stream *stm, std::string txClassName, std::string featureName);
    void ReadTxInfo        (Stream *stm, std::string txClassName, std::string featureName);
    void WritePrimary      (Stream *stm,  ByteArray publishingLicense);
    void ReadPrimary       (Stream *stm, ByteArray &publishingLicense);

    bool isMetro = true;
    std::string drmContent          = "\11DRMContent";
    std::string metroContent        = "EncryptedPackage";
    std::string dataSpace           = "\006DataSpaces";
    std::string version             = "Version";
    std::string dataSpaceMap        = "DataSpaceMap";
    std::string dataSpaceInfo       = "DataSpaceInfo";
    std::string drmDataSpace        = "\11DRMDataSpace";
    std::string metroDataSpace      = "DRMEncryptedDataSpace";
    std::string transformInfo       = "TransformInfo";
    std::string drmTransform        = "\11DRMTransform";
    std::string metroTransform      = "DRMEncryptedTransform";
    std::string passwordTransform   = "StrongEncryptionTransform";
    std::string passwordDataSpace   = "StrongEncryptionDataSpace";
    std::string primary             = "\006Primary";
    std::string versionFeature      = "Microsoft.Container.DataSpaces";
    std::string drmTransformFeature = "Microsoft.Metadata.DRMTransform";
    std::string drmTransformClass   = "{C73DFACD-061F-43B0-8B64-0C620D2A8B50}";
};

} // namespace officeprotector
} //namespace rmscore

#endif

