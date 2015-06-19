/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IDOMATTRIBUTEQTIMPL_H
#define IDOMATTRIBUTEQTIMPL_H

#include "IDomAttribute.h"
#include <QDomAttr>

class DomAttributeQt : public IDomAttribute
{
private:
    std::string	name() const override;
    sp<IDomElement>	ownerElement() const override;
    std::string	value() const override;
// form IDomNode
    sp<DomNamedNodeMap>	attributes() const override;
    sp<DomNodeList>	childNodes() const override;

    bool	hasAttributes() const override;
    bool	hasChildNodes() const override;

    bool	isAttr() const override;
    bool	isDocument() const override;
    bool	isElement() const override;
    bool	isNull() const override;
    bool	isText() const override;

    std::string         localName() const override;
    sp<IDomNode>        namedItem(const std::string & name) const override;
    std::string         namespaceURI() const override;
    std::string         nodeName() const override;
    NodeType            nodeType() const override;
    std::string         nodeValue() const override;
    sp<IDomDocument>	ownerDocument() const override;

    sp<IDomNode>        firstChild() const override;
    sp<IDomElement>     firstChildElement(const std::string & tagName = std::string()) const override;
    sp<IDomNode>        lastChild() const override;
    sp<IDomElement>     lastChildElement(const std::string & tagName = std::string()) const override;
    sp<IDomNode>        previousSibling() const override;
    sp<IDomElement>     previousSiblingElement(const std::string & tagName = std::string()) const override;
    sp<IDomNode>        nextSibling() const override;
    sp<IDomElement>     nextSiblingElement(const std::string & tagName = std::string()) const override;
    sp<IDomNode>        parentNode() const override;
    std::string         prefix() const override;

    sp<IDomAttribute>   toAttr() const override;
    sp<IDomDocument>	toDocument() const override;
    sp<IDomElement>     toElement() const override;
    std::string         toText() const override;

public:
    DomAttributeQt(const QDomAttr& other):impl_(other){}

private:
    QDomAttr impl_;
//    friend class DomNodeQt;
//    friend class DomDocumentQt;
//    friend class DomElementQt;
};

#endif // IDOMATTRIBUTEQTIMPL_H
