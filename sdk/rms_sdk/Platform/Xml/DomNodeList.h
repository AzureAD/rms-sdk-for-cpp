/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef DOMNODELIST
#define DOMNODELIST

#include <list>
#include <memory>

class IDomNode;
using DomNodeList = std::list<std::shared_ptr<IDomNode>>;

#endif // DOMNODELIST

