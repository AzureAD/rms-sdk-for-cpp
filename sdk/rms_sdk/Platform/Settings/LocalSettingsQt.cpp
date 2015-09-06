/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifdef QTFRAMEWORK
#include <QDebug>
#include "LocalSettingsQt.h"

namespace rmscore {
namespace platform {
namespace settings {

std::shared_ptr<ILocalSettings> ILocalSettings::Create(const std::string& filename)
{
    return std::make_shared<LocalSettingsQt>(filename.c_str());
}

LocalSettingsQt::LocalSettingsQt(const QString& filename) :
  impl_(filename, QSettings::IniFormat) {}

std::string LocalSettingsQt::GetString(const std::string& container,
                                            const std::string& name,
                                            const std::string& defaultValue)
{
  std::string key(container.empty() ? name : container + "/" + name);

  return this->impl_.value(key.c_str(),
                           defaultValue.c_str()).toString().toStdString();
}

bool LocalSettingsQt::GetBool(const std::string& container,
                                   const std::string& name,
                                   bool               bDefaultValue)
{
  std::string key(container.empty() ? name : container + "/" + name);

  return this->impl_.value(key.c_str(), bDefaultValue).toBool();
}

void LocalSettingsQt::SetBool(const std::string& container,
                                   const std::string& name,
                                   bool               bValue)
{
  std::string key(container.empty() ? name : container + "/" + name);

  return this->impl_.setValue(key.c_str(), bValue);
}

int LocalSettingsQt::GetInt(const std::string& container,
                                 const std::string& name,
                                 int                nDefaultValue)
{
  std::string key(container.empty() ? name : container + "/" + name);

  return this->impl_.value(key.c_str(), nDefaultValue).toInt();
}

void LocalSettingsQt::SetInt(const std::string& container,
                                  const std::string& name,
                                  int                nValue)
{
  std::string key(container.empty() ? name : container + "/" + name);

  return this->impl_.setValue(key.c_str(), nValue);
}
}
}
} // namespace rmscore { namespace platform { namespace settings {

#endif
