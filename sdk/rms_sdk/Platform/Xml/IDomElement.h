/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IDOMELEMENT_H
#define IDOMELEMENT_H

#include <string>
#include "IDomNode.h"

class IDomAttribute;

class IDomElement : public IDomNode
{
public:
    virtual std::string	attribute(const std::string & name, const std::string & defValue = std::string()) const = 0;
    virtual std::string	attributeNS(const std::string nsURI, const std::string & localName, const std::string & defValue = std::string()) const = 0;
    virtual sp<IDomAttribute>	attributeNode(const std::string & name) = 0;
    virtual sp<IDomAttribute>	attributeNodeNS(const std::string & nsURI, const std::string & localName) = 0;
    virtual sp<DomNodeList>	elementsByTagName(const std::string & tagname) const = 0;
    virtual sp<DomNodeList>	elementsByTagNameNS(const std::string & nsURI, const std::string & localName) const = 0;
    virtual bool	hasAttribute(const std::string & name) const = 0;
    virtual bool	hasAttributeNS(const std::string & nsURI, const std::string & localName) const = 0;
    virtual std::string	tagName() const = 0;
    virtual std::string	text() const = 0;
};
#endif // IDOMELEMENT_H
