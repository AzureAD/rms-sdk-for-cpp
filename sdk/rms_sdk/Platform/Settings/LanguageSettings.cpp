/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include"LanguageSettings.h"

namespace rmscore { namespace platform { namespace settings {

std::vector<std::string> LanguageSettings::GetAppLanguages()
{
    std::vector<std::string> res;
    res.push_back("En-en");
    return res;
}

std::shared_ptr<ILanguageSettings> ILanguageSettings::Create()
{
    return std::make_shared<LanguageSettings>();
}

}}} //namespace rmscore { namespace platform { namespace settings {
