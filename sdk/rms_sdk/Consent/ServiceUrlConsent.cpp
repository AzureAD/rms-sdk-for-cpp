/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "ServiceUrlConsent.h"
#include "../ModernAPI/ConsentType.h"

namespace rmscore { namespace consent {

ServiceUrlConsent::ServiceUrlConsent(const std::string& email, const std::vector<std::string> &urls)
{
    for_each(
        urls.begin(), urls.end(),
        [&](std::string url)
        {
            auto urlProtocolLength = url.length();

            if (urlProtocolLength > 4) urlProtocolLength = 4;
            std::string urlProtocol = url.substr(0, urlProtocolLength);

            std::transform(urlProtocol.begin(), urlProtocol.end(),
                           urlProtocol.begin(), tolower);

            if (urlProtocol.compare("http") != 0) url.insert(0, "https://");

            this->urls_.push_back(url);
        });

    this->user_ = email;
    this->type_ = modernapi::ConsentType::ServiceUrlConsent;
}

} // namespace consent
} // namespace rmscore

