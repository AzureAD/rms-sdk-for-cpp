/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IDOMATTRIBUTE_H
#define IDOMATTRIBUTE_H

#include <string>
#include "IDomNode.h"

class IDomElement;

class IDomAttribute : public IDomNode
{
public:
    virtual std::string	name() const= 0;
    virtual sp<IDomElement>	ownerElement() const= 0;
    virtual std::string	value() const= 0;
};

#endif // IDOMATTRIBUTE_H
