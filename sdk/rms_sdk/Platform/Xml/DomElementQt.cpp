/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifdef QTFRAMEWORK
#include "DomElementQt.h"
#include "DomAttributeQt.h"
#include "DomNodeQt.h"
#include "DomDocumentQt.h"
#include <QDomAttr>
#include <QDomNode>
#include <QString>
#include <QDomNamedNodeMap>

std::string	DomElementQt::attribute(const std::string & name, const std::string & defValue/* = std::string()*/) const
{
    return this->impl_.attribute(QString::fromStdString(name), QString::fromStdString(defValue)).toStdString();
}
std::string	DomElementQt::attributeNS(const std::string nsURI, const std::string & localName, const std::string & defValue/* = std::string()*/) const
{
    return this->impl_.attributeNS(QString::fromStdString(nsURI), localName.c_str(), defValue.c_str()).toStdString();
}
sp<IDomAttribute>	DomElementQt::attributeNode(const std::string & name)
{
    QDomAttr attr = this->impl_.attributeNode(name.c_str());
    return  std::make_shared<DomAttributeQt>(attr);
}
sp<IDomAttribute>	DomElementQt::attributeNodeNS(const std::string & nsURI, const std::string & localName)
{
    QDomAttr attr = this->impl_.attributeNodeNS(nsURI.c_str(), localName.c_str());
    return  std::make_shared<DomAttributeQt>(attr);
}
//sp<DomNamedNodeMap>	DomElementQt::attributes() const
//{
//    QDomNamedNodeMap map = this->impl_.attributes();
//    DomNamedNodeMap* pRes = new DomNamedNodeMap();
//    for(int i = 0; i < map.size(); ++i)
//    {
//        QDomNode& node = map.item(i);
//        pRes->insert(std::make_pair(node.nodeName().toStdString(),  std::make_shared<DomNodeQt>(node)));
//    }

//    return sp<DomNamedNodeMap>(pRes);
//}
sp<DomNodeList>	DomElementQt::elementsByTagName(const std::string & tagname) const
{
    QDomNodeList list = this->impl_.elementsByTagName(QString::fromStdString(tagname));
    DomNodeList* pRes = new DomNodeList();
    for(int i = 0; i < list.size(); ++i)
    {
        QDomNode node = list.item(i);
        pRes->push_back( std::make_shared<DomNodeQt>(node));
    }

    return sp<DomNodeList>(pRes);
}
sp<DomNodeList>	DomElementQt::elementsByTagNameNS(const std::string & nsURI, const std::string & localName) const
{
    QDomNodeList list = this->impl_.elementsByTagNameNS(QString::fromStdString(nsURI), QString::fromStdString(localName));
    DomNodeList* pRes = new DomNodeList();
    for(int i = 0; i < list.size(); ++i)
    {
        QDomNode node = list.item(i);
        pRes->push_back( std::make_shared<DomNodeQt>(node));
    }

    return sp<DomNodeList>(pRes);
}
bool	DomElementQt::hasAttribute(const std::string & name) const
{
    return this->impl_.hasAttribute(QString::fromStdString(name));
}
bool	DomElementQt::hasAttributeNS(const std::string & nsURI, const std::string & localName) const
{
    return this->impl_.hasAttributeNS(QString::fromStdString(nsURI), QString::fromStdString(localName));
}
//IDomNode::NodeType	DomElementQt::nodeType() const
//{
//    return (IDomNode::NodeType)this->impl_.nodeType();
//}
std::string	DomElementQt::tagName() const
{
    return this->impl_.tagName().toStdString();
}
std::string	DomElementQt::text() const
{
    return this->impl_.text().toStdString();
}

// from IDomNode

sp<DomNamedNodeMap>	DomElementQt::attributes() const
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
sp<DomNodeList>	DomElementQt::childNodes() const
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
bool	DomElementQt::hasAttributes() const
{
    return this->impl_.hasAttributes();
}
bool	DomElementQt::hasChildNodes() const
{
    return this->impl_.hasChildNodes();
}
bool	DomElementQt::isAttr() const
{
    return this->impl_.isAttr();
}
bool	DomElementQt::isDocument() const
{
    return this->impl_.isDocument();
}
bool	DomElementQt::isElement() const
{
    return this->impl_.isElement();
}
bool	DomElementQt::isNull() const
{
    return this->impl_.isNull();
}
bool	DomElementQt::isText() const
{
    return this->impl_.isText();
}
std::string     DomElementQt::localName() const
{
    return this->impl_.localName().toStdString();
}
sp<IDomNode>        DomElementQt::namedItem(const std::string & name) const
{
    QDomNode node = this->impl_.namedItem(QString::fromStdString(name));
    return  std::make_shared<DomNodeQt>(node);
}
std::string     DomElementQt::namespaceURI() const
{
    return this->impl_.namespaceURI().toStdString();
}
std::string     DomElementQt::nodeName() const
{
    return this->impl_.nodeName().toStdString();
}
IDomNode::NodeType        DomElementQt::nodeType() const
{
    return (IDomNode::NodeType)this->impl_.nodeType();
}
std::string     DomElementQt::nodeValue() const
{
    return this->impl_.nodeValue().toStdString();
}
sp<IDomDocument>	DomElementQt::ownerDocument() const
{
    QDomDocument doc = this->impl_.ownerDocument();
    return  std::make_shared<DomDocumentQt>(doc);
}
sp<IDomNode>        DomElementQt::firstChild() const
{
    QDomNode node = this->impl_.firstChild();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomElementQt::firstChildElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.firstChildElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomElementQt::lastChild() const
{
    QDomNode node = this->impl_.lastChild();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomElementQt::lastChildElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.lastChildElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomElementQt::previousSibling() const
{
    QDomNode node = this->impl_.previousSibling();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomElementQt::previousSiblingElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.previousSiblingElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomElementQt::nextSibling() const
{
    QDomNode node = this->impl_.nextSibling();
    return  std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomElementQt::nextSiblingElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.nextSiblingElement(QString::fromStdString(tagName));
    return  std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomElementQt::parentNode() const
{
    QDomNode node = this->impl_.parentNode();
    return  std::make_shared<DomNodeQt>(node);

}
std::string     DomElementQt::prefix() const
{
    return impl_.prefix().toStdString();
}
sp<IDomAttribute>   DomElementQt::toAttr() const
{
    QDomAttr attr = this->impl_.toAttr();
    return  std::make_shared<DomAttributeQt>(attr);
}
sp<IDomDocument>	DomElementQt::toDocument() const
{
    QDomDocument doc = this->impl_.toDocument();
    return  std::make_shared<DomDocumentQt>(doc);
}
sp<IDomElement>     DomElementQt::toElement() const
{
    QDomElement elem = this->impl_.toElement();
    return  std::make_shared<DomElementQt>(elem);
}
std::string     DomElementQt::toText() const
{
    return this->impl_.toText().data().toStdString();
}

#endif
