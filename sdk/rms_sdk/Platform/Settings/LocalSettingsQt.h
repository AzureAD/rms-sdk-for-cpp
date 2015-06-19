/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef LOCALSETTINGSQTIMPL
#define LOCALSETTINGSQTIMPL

#include"ILocalSettings.h"
#include <QSettings>

namespace rmscore { namespace platform { namespace settings {

class LocalSettingsQt : public ILocalSettings
{
public:
    LocalSettingsQt(const QString& filename);
    virtual std::string GetString(const std::string& container, const std::string& name, const std::string& defaultValue) override;
    virtual bool GetBool(const std::string& container, const std::string& name, bool bDefaultValue) override;
    virtual void SetBool(const std::string& container, const std::string& name, bool bValue) override;

    virtual int GetInt(const std::string& container, const std::string& name, int nDefaultValue) override;
    virtual void SetInt(const std::string& container, const std::string& name, int nValue) override;

private:
    QSettings impl_;
};

}}} // namespace rmscore { namespace platform { namespace settings {

#endif // LOCALSETTINGSQTIMPL

