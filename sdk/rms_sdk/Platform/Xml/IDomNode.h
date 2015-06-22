/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IDOMNODE_H
#define IDOMNODE_H

#include <string>
#include <memory>
#include "../../Common/CommonTypes.h"
#include "DomNamedNodeMap.h"
#include "DomNodeList.h"

template <typename T>
using sp = std::shared_ptr<T>;

class IDomDocument;
class IDomAttribute;
class IDomElement;

class IDomNode
{
public:
    enum class NodeType
    {
        ElementNode = 1,
        AttributeNode = 2,
        TextNode = 3,
        DocumentNode = 10,
    };

public:
    virtual sp<DomNamedNodeMap>	attributes() const= 0;
    virtual sp<DomNodeList>	childNodes() const= 0;

    virtual bool	hasAttributes() const= 0;
    virtual bool	hasChildNodes() const= 0;

    virtual bool	isAttr() const= 0;
    virtual bool	isDocument() const= 0;
    virtual bool	isElement() const= 0;
    virtual bool	isNull() const= 0;
    virtual bool	isText() const= 0;

    virtual std::string         localName() const= 0;
    virtual sp<IDomNode>        namedItem(const std::string & name) const= 0;
    virtual std::string         namespaceURI() const= 0;
    virtual std::string         nodeName() const = 0;
    virtual NodeType            nodeType() const = 0;
    virtual std::string         nodeValue() const = 0;
    virtual sp<IDomDocument>	ownerDocument() const = 0;

    virtual sp<IDomNode>        firstChild() const = 0;
    virtual sp<IDomElement>     firstChildElement(const std::string & tagName = std::string()) const = 0;
    virtual sp<IDomNode >       lastChild() const = 0;
    virtual sp<IDomElement>     lastChildElement(const std::string & tagName = std::string()) const = 0;
    virtual sp<IDomNode>        previousSibling() const= 0;
    virtual sp<IDomElement>     previousSiblingElement(const std::string & tagName = std::string()) const = 0;
    virtual sp<IDomNode>        nextSibling() const= 0;
    virtual sp<IDomElement>     nextSiblingElement(const std::string & tagName = std::string()) const = 0;
    virtual sp<IDomNode>        parentNode() const = 0;
    virtual std::string         prefix() const = 0;

    virtual sp<IDomAttribute>   toAttr() const = 0;
    virtual sp<IDomDocument>	toDocument() const = 0;
    virtual sp<IDomElement>     toElement() const = 0;
    virtual std::string         toText() const = 0;
};

using DomNamedNodeMap = rmscore::modernapi::detail::HashMapString<sp<IDomNode>>;
#include <list>
using DomNodeList = std::list<sp<IDomNode>>;

#endif // IDOMNODE_H
