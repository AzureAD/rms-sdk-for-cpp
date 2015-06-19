/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IDOMELEMENTQTIMPL_H
#define IDOMELEMENTQTIMPL_H

#include "IDomElement.h"
#include <QDomElement>

class DomElementQt : public IDomElement
{
private:
    std::string	attribute(const std::string & name, const std::string & defValue = std::string()) const override;
    std::string	attributeNS(const std::string nsURI, const std::string & localName, const std::string & defValue = std::string()) const override;
    sp<IDomAttribute>	attributeNode(const std::string & name) override;
    sp<IDomAttribute>	attributeNodeNS(const std::string & nsURI, const std::string & localName) override;
    sp<DomNodeList>	elementsByTagName(const std::string & tagname) const override;
    sp<DomNodeList>	elementsByTagNameNS(const std::string & nsURI, const std::string & localName) const override;
    bool	hasAttribute(const std::string & name) const override;
    bool	hasAttributeNS(const std::string & nsURI, const std::string & localName) const override;
    std::string	tagName() const override;
    std::string	text() const override;

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
    DomElementQt(const QDomElement& other):impl_(other){}

private:
    QDomElement impl_;
//    friend class DomNodeQt;
//    friend class DomDocumentQt;
//    friend class DomAttributeQt;

};

#endif // IDOMELEMENTQTIMPL_H
