/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef ILOCALSETTINGS
#define ILOCALSETTINGS

#include<memory>
#include<string>

namespace rmscore { namespace platform { namespace settings {

class ILocalSettings
{
public:
    virtual std::string GetString(const std::string& container, const std::string& name, const std::string& defaultValue) = 0;
    virtual bool GetBool(const std::string& container, const std::string& name, bool bDefaultValue) = 0;
    virtual void SetBool(const std::string& container, const std::string& name, bool bValue) = 0;

    virtual int GetInt(const std::string& container, const std::string& name, int nDefaultValue) = 0;
    virtual void SetInt(const std::string& container, const std::string& name, int nValue) = 0;

public:
    static std::shared_ptr<ILocalSettings> Create(const std::string& filename = "appConfig.cfg");
};

}}} //namespace rmscore { namespace platform { namespace settings {
#endif // ILOCALSETTINGS

