/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef DOMNAMEDNODEMAP
#define DOMNAMEDNODEMAP

#include <string>
#include <memory>
#include "../../Common/CommonTypes.h"

class IDomNode;
using DomNamedNodeMap = rmscore::common::HashMapString<std::shared_ptr<IDomNode>>;

#endif // DOMNAMEDNODEMAP

