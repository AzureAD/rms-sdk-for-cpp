/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifdef QTFRAMEWORK
#include "DomNodeQt.h"
#include "DomDocumentQt.h"
#include "DomAttributeQt.h"
#include "DomDocumentQt.h"
#include "DomElementQt.h"

sp<DomNamedNodeMap>	DomNodeQt::attributes() const
{
    QDomNamedNodeMap map = this->impl_.attributes();
    DomNamedNodeMap* pRes = new DomNamedNodeMap();
    for(int i = 0; i < map.size(); ++i)
    {
        QDomNode node = map.item(i);
        pRes->insert(std::make_pair(node.nodeName().toStdString(),  std::make_shared<DomNodeQt>(node)));
    }

    return sp<DomNamedNodeMap>(pRes);
}
sp<DomNodeList>	DomNodeQt::childNodes() const
{
    QDomNodeList list = this->impl_.childNodes();
    DomNodeList* pRes = new DomNodeList();
    for(int i = 0; i < list.size(); ++i)
    {
        QDomNode node = list.item(i);
        pRes->push_back( std::make_shared<DomNodeQt>(node));
    }

    return sp<DomNodeList>(pRes);
}
bool	DomNodeQt::hasAttributes() const
{
    return this->impl_.hasAttributes();
}
bool	DomNodeQt::hasChildNodes() const
{
    return this->impl_.hasChildNodes();
}
bool	DomNodeQt::isAttr() const
{
    return this->impl_.isAttr();
}
bool	DomNodeQt::isDocument() const
{
    return this->impl_.isDocument();
}
bool	DomNodeQt::isElement() const
{
    return this->impl_.isElement();
}
bool	DomNodeQt::isNull() const
{
    return this->impl_.isNull();
}
bool	DomNodeQt::isText() const
{
    return this->impl_.isText();
}
std::string     DomNodeQt::localName() const
{
    return this->impl_.localName().toStdString();
}
sp<IDomNode>        DomNodeQt::namedItem(const std::string & name) const
{
    QDomNode node = this->impl_.namedItem(QString::fromStdString(name));
    return  std::make_shared<DomNodeQt>(node);
}
std::string     DomNodeQt::namespaceURI() const
{
    return this->impl_.namespaceURI().toStdString();
}
std::string     DomNodeQt::nodeName() const
{
    return this->impl_.nodeName().toStdString();
}
IDomNode::NodeType        DomNodeQt::nodeType() const
{
    return (IDomNode::NodeType)this->impl_.nodeType();
}
std::string     DomNodeQt::nodeValue() const
{
    return this->impl_.nodeValue().toStdString();
}
sp<IDomDocument>	DomNodeQt::ownerDocument() const
{
    QDomDocument doc = this->impl_.ownerDocument();
    return  std::make_shared<DomDocumentQt>(doc);
}
sp<IDomNode>        DomNodeQt::firstChild() const
{
    QDomNode node = this->impl_.firstChild();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomNodeQt::firstChildElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.firstChildElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomNodeQt::lastChild() const
{
    QDomNode node = this->impl_.lastChild();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomNodeQt::lastChildElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.lastChildElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomNodeQt::previousSibling() const
{
    QDomNode node = this->impl_.previousSibling();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomNodeQt::previousSiblingElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.previousSiblingElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomNodeQt::nextSibling() const
{
    QDomNode node = this->impl_.nextSibling();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomNodeQt::nextSiblingElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.nextSiblingElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomNodeQt::parentNode() const
{
    QDomNode node = this->impl_.parentNode();
    return  std::make_shared<DomNodeQt>(node);

}
std::string     DomNodeQt::prefix() const
{
    return impl_.prefix().toStdString();
}
sp<IDomAttribute>   DomNodeQt::toAttr() const
{
    QDomAttr attr = this->impl_.toAttr();
    return sp<IDomAttribute>(new DomAttributeQt(attr));
}
sp<IDomDocument>	DomNodeQt::toDocument() const
{
    QDomDocument doc = this->impl_.toDocument();
    return  std::make_shared<DomDocumentQt>(doc);
}
sp<IDomElement>     DomNodeQt::toElement() const
{
    QDomElement elem = this->impl_.toElement();
    return  std::make_shared<DomElementQt>(elem);
}
std::string     DomNodeQt::toText() const
{
    return this->impl_.toText().data().toStdString();
}
#endif
