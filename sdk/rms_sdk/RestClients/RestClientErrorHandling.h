/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_RESTCLIENTERRORHANDLING_H_
#define _RMS_RESTCLIENTERRORHANDLING_H_
#include <string>
#include "../Platform/Http/IHttpClient.h"

namespace rmscore { namespace restclients {

void HandleRestClientError(platform::http::StatusCode httpStatusCode, rmscore::common::ByteArray &sResponse);

} // namespace restclients
} // namespace rmscore
#endif // _RMS_RESTCLIENTERRORHANDLING_H_
