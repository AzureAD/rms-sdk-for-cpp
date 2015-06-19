/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifdef QTFRAMEWORK
#include "DomAttributeQt.h"
#include "DomElementQt.h"
#include "DomNodeQt.h"
#include "DomDocumentQt.h"

std::string	DomAttributeQt::name() const
{
    return this->impl_.name().toStdString();
}
//IDomNode::NodeType	DomAttributeQt::nodeType() const
//{
//    return (IDomNode::NodeType)this->impl_.nodeType();
//}
sp<IDomElement>	DomAttributeQt::ownerElement() const
{
    QDomElement elem = this->impl_.ownerElement();
    return  std::make_shared<DomElementQt>(elem);
}
std::string	DomAttributeQt::value() const
{
    return this->impl_.value().toStdString();
}

// from IDomNode

sp<DomNamedNodeMap>	DomAttributeQt::attributes() const
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
sp<DomNodeList>	DomAttributeQt::childNodes() const
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
bool	DomAttributeQt::hasAttributes() const
{
    return this->impl_.hasAttributes();
}
bool	DomAttributeQt::hasChildNodes() const
{
    return this->impl_.hasChildNodes();
}
bool	DomAttributeQt::isAttr() const
{
    return this->impl_.isAttr();
}
bool	DomAttributeQt::isDocument() const
{
    return this->impl_.isDocument();
}
bool	DomAttributeQt::isElement() const
{
    return this->impl_.isElement();
}
bool	DomAttributeQt::isNull() const
{
    return this->impl_.isNull();
}
bool	DomAttributeQt::isText() const
{
    return this->impl_.isText();
}
std::string     DomAttributeQt::localName() const
{
    return this->impl_.localName().toStdString();
}
sp<IDomNode>        DomAttributeQt::namedItem(const std::string & name) const
{
    QDomNode node = this->impl_.namedItem(QString::fromStdString(name));
    return  std::make_shared<DomNodeQt>(node);
}
std::string     DomAttributeQt::namespaceURI() const
{
    return this->impl_.namespaceURI().toStdString();
}
std::string     DomAttributeQt::nodeName() const
{
    return this->impl_.nodeName().toStdString();
}
IDomNode::NodeType        DomAttributeQt::nodeType() const
{
    return (IDomNode::NodeType)this->impl_.nodeType();
}
std::string     DomAttributeQt::nodeValue() const
{
    return this->impl_.nodeValue().toStdString();
}
sp<IDomDocument>	DomAttributeQt::ownerDocument() const
{
    QDomDocument doc = this->impl_.ownerDocument();
    return  std::make_shared<DomDocumentQt>(doc);
}
sp<IDomNode>        DomAttributeQt::firstChild() const
{
    QDomNode node = this->impl_.firstChild();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomAttributeQt::firstChildElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.firstChildElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomAttributeQt::lastChild() const
{
    QDomNode node = this->impl_.lastChild();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomAttributeQt::lastChildElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.lastChildElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomAttributeQt::previousSibling() const
{
    QDomNode node = this->impl_.previousSibling();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomAttributeQt::previousSiblingElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.previousSiblingElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomAttributeQt::nextSibling() const
{
    QDomNode node = this->impl_.nextSibling();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomAttributeQt::nextSiblingElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.nextSiblingElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomAttributeQt::parentNode() const
{
    QDomNode node = this->impl_.parentNode();
    return  std::make_shared<DomNodeQt>(node);

}
std::string     DomAttributeQt::prefix() const
{
    return impl_.prefix().toStdString();
}
sp<IDomAttribute>   DomAttributeQt::toAttr() const
{
    QDomAttr attr = this->impl_.toAttr();
    return  std::make_shared<DomAttributeQt>(attr);
}
sp<IDomDocument>	DomAttributeQt::toDocument() const
{
    QDomDocument doc = this->impl_.toDocument();
    return  std::make_shared<DomDocumentQt>(doc);
}
sp<IDomElement>     DomAttributeQt::toElement() const
{
    QDomElement elem = this->impl_.toElement();
    return  std::make_shared<DomElementQt>(elem);
}
std::string     DomAttributeQt::toText() const
{
    return this->impl_.toText().data().toStdString();
}

#endif

