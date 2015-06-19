/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IDOMDOCUMENTQTIPL_H
#define IDOMDOCUMENTQTIPL_H

#include "IDomDocument.h"
#include <QDomDocument>
#include <memory>

class DomDocumentQt : public IDomDocument
{
private:
    sp<IDomElement>	documentElement() const;
    sp<IDomElement>	elementById(const std::string & elementId);
    sp<DomNodeList> elementsByTagName(const std::string & tagname) const;

    bool setContent(const std::string & text, bool namespaceProcessing, std::string & errorMsg, int & errorLine, int & errorColumn) override;
    bool setContent(const std::string & text, std::string & errorMsg, int & errorLine, int & errorColumn) override;

    sp<IDomElement> SelectSingleNode(const std::string &xPath) override;

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
    DomDocumentQt(const QDomDocument& other):impl_(other){}
    DomDocumentQt():impl_(){}

public:
    QDomDocument impl_;
//    friend class IDomDocument;
//    friend class DomNodeQt;
//    friend class DomAttributeQt;
//    friend class DomElementQt;

};

#endif // IDOMDOCUMENTQTIPL_H
