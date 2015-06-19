/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <RmsauthIdHelper.h>
#include <QSysInfo>
#include <QString>

namespace rmsauth {

const String RmsauthIdHelper::getProcessorArchitecture()
{
#ifdef QT_VER_LESS_THEN_54
    return "unk";
#else
    return QSysInfo::currentCpuArchitecture().toStdString();
#endif
}
const String RmsauthIdHelper::getOSVersion()
{
#ifdef QT_VER_LESS_THEN_54
    return "unk";
#else
    return QSysInfo::kernelType().toStdString();
#endif
}

} // namespace rmsauth {
