/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef ILANGUAGESETTINGS
#define ILANGUAGESETTINGS

#include<memory>
#include<string>
#include<vector>

namespace rmscore { namespace platform { namespace settings {

class ILanguageSettings
{
public:
    virtual std::vector<std::string> GetAppLanguages() = 0;

public:
    static std::shared_ptr<ILanguageSettings> Create();
};

}}} // namespace rmscore { namespace platform { namespace settings {

#endif // ILANGUAGESETTINGS

