/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IWEBUI_H
#define IWEBUI_H

#include "types.h"
#include "Url.h"
#include "PromptBehavior.h"

namespace rmsauth {

class IWebUI
{
public:
    virtual String authenticate(const String& requestUri, const String& callbackUri) = 0;
};
using IWebUIPtr = ptr<IWebUI>;

class WebUI : public IWebUI
{
    PromptBehavior promptBehavior_;

public:
    WebUI(PromptBehavior promptBehavior);

    String authenticate(const String& requestUri, const String& callbackUri) override;
};

} // namespace rmsauth {

#endif // IWEBUI_H
