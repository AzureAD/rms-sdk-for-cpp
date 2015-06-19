/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef LANGUAGESETTINGS
#define LANGUAGESETTINGS

#include"ILanguageSettings.h"
namespace rmscore { namespace platform { namespace settings {
class LanguageSettings : public ILanguageSettings
{
public:
    virtual std::vector<std::string> GetAppLanguages() override;
};
}}} //namespace rmscore { namespace platform { namespace settings {
#endif // LANGUAGESETTINGS

