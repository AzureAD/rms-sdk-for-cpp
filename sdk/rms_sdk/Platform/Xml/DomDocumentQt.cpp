/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifdef QTFRAMEWORK
#include "DomDocumentQt.h"
#include "DomNodeQt.h"
#include "DomAttributeQt.h"
#include "DomElementQt.h"
#include <QDomNodeList>
#include <QXmlQuery>
#include <QBuffer>
#include "../Logger/Logger.h"

using namespace rmscore::platform::logger;

sp<IDomDocument> IDomDocument::create()
{
    return sp<IDomDocument>(new DomDocumentQt());
}
bool DomDocumentQt::setContent(const std::string & text, bool namespaceProcessing, std::string & errorMsg, int & errorLine, int & errorColumn)
{
    QString str;
    bool res = this->impl_.setContent(QString(text.c_str()), namespaceProcessing, &str, &errorLine, &errorColumn);
    errorMsg = str.toStdString();
    return res;
}
bool DomDocumentQt::setContent(const std::string & text, std::string & errorMsg, int & errorLine, int & errorColumn)
{
    QString str;
    bool res = this->impl_.setContent(QString(text.c_str()), &str, &errorLine, &errorColumn);
    errorMsg = str.toStdString();
    return res;
}

sp<IDomElement>	DomDocumentQt::documentElement() const
{
    QDomElement elem = this->impl_.documentElement();
    return std::make_shared<DomElementQt>(elem);

}

sp<IDomElement>	DomDocumentQt::elementById(const std::string & elementId)
{
    QDomElement elem = this->impl_.elementById(elementId.c_str());
    return std::make_shared<DomElementQt>(elem);
}

sp<DomNodeList> DomDocumentQt::elementsByTagName(const std::string & tagname) const
{
    QDomNodeList list = this->impl_.elementsByTagName(tagname.c_str());
    DomNodeList *pRes = new DomNodeList();
    for(int i = 0; i< list.size(); ++i)
    {
        QDomNode node = list.at(i);
        pRes->push_back(std::make_shared<DomNodeQt>(node));
    }
    return sp<DomNodeList>(pRes);
}

sp<IDomElement> DomDocumentQt::SelectSingleNode(const std::string &xPath)
{
    const QString defaultNsPattern = "declare default element namespace '%1'";
    const QString xpathReqPattern = "%1; %2/%3";

    QBuffer device;
    device.setData(this->impl_.toString().toUtf8());
    device.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("inputDocument", &device);

    auto defaultNs = this->impl_.documentElement().toElement().attribute("xmlns");
    Logger::Hidden("DomDocumentQt::SelectSingleNode: defaultNs: %s", defaultNs.toStdString().data());

    QString  xpathReq = xpathReqPattern.arg(defaultNsPattern.arg(defaultNs)).arg("doc($inputDocument)").arg(xPath.data());
    query.setQuery(xpathReq);
    Logger::Hidden("DomDocumentQt::SelectSingleNode: xpathReq: %s", xpathReq.toStdString().data());

    if ( !query.isValid())
    {
        Logger::Error("Error: DomDocumentQt::SelectSingleNode: invalid query.");
        return nullptr;
    }
    QString res;
    query.evaluateTo(&res);
    if(res.isEmpty())
    {
        Logger::Warning("DomDocumentQt::SelectSingleNode: result is empty.");
        return nullptr;
    }
    QDomDocument resDoc;
    resDoc.setContent("<result>" + res + "</result>");

    auto root = resDoc.documentElement();

    Logger::Hidden("DomDocumentQt::SelectSingleNode: resDoc: %s", res.toStdString().data());
    return std::make_shared<DomElementQt>(resDoc.documentElement());
}

// from IDomNode

sp<DomNamedNodeMap>	DomDocumentQt::attributes() const
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
sp<DomNodeList>	DomDocumentQt::childNodes() const
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
bool	DomDocumentQt::hasAttributes() const
{
    return this->impl_.hasAttributes();
}
bool	DomDocumentQt::hasChildNodes() const
{
    return this->impl_.hasChildNodes();
}
bool	DomDocumentQt::isAttr() const
{
    return this->impl_.isAttr();
}
bool	DomDocumentQt::isDocument() const
{
    return this->impl_.isDocument();
}
bool	DomDocumentQt::isElement() const
{
    return this->impl_.isElement();
}
bool	DomDocumentQt::isNull() const
{
    return this->impl_.isNull();
}
bool	DomDocumentQt::isText() const
{
    return this->impl_.isText();
}
std::string     DomDocumentQt::localName() const
{
    return this->impl_.localName().toStdString();
}
sp<IDomNode>        DomDocumentQt::namedItem(const std::string & name) const
{
    QDomNode node = this->impl_.namedItem(QString::fromStdString(name));
    return std::make_shared<DomNodeQt>(node);
}
std::string     DomDocumentQt::namespaceURI() const
{
    return this->impl_.namespaceURI().toStdString();
}
std::string     DomDocumentQt::nodeName() const
{
    return this->impl_.nodeName().toStdString();
}
IDomNode::NodeType        DomDocumentQt::nodeType() const
{
    return (IDomNode::NodeType)this->impl_.nodeType();
}
std::string     DomDocumentQt::nodeValue() const
{
    return this->impl_.nodeValue().toStdString();
}
sp<IDomDocument>	DomDocumentQt::ownerDocument() const
{
    QDomDocument doc = this->impl_.ownerDocument();
    return  std::make_shared<DomDocumentQt>(doc);
}
sp<IDomNode>        DomDocumentQt::firstChild() const
{
    QDomNode node = this->impl_.firstChild();
    return std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomDocumentQt::firstChildElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.firstChildElement(QString::fromStdString(tagName));
    return std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomDocumentQt::lastChild() const
{
    QDomNode node = this->impl_.lastChild();
    return std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomDocumentQt::lastChildElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.lastChildElement(QString::fromStdString(tagName));
    return std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomDocumentQt::previousSibling() const
{
    QDomNode node = this->impl_.previousSibling();
    return std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomDocumentQt::previousSiblingElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.previousSiblingElement(QString::fromStdString(tagName));
    return std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomDocumentQt::nextSibling() const
{
    QDomNode node = this->impl_.nextSibling();
    return std::make_shared<DomNodeQt>(node);
}
sp<IDomElement>     DomDocumentQt::nextSiblingElement(const std::string & tagName/* = std::string()*/) const
{
    QDomElement elem = this->impl_.nextSiblingElement(QString::fromStdString(tagName));
    return std::make_shared<DomElementQt>(elem);
}
sp<IDomNode>        DomDocumentQt::parentNode() const
{
    QDomNode node = this->impl_.parentNode();
    return std::make_shared<DomNodeQt>(node);

}
std::string     DomDocumentQt::prefix() const
{
    return impl_.prefix().toStdString();
}
sp<IDomAttribute>   DomDocumentQt::toAttr() const
{
    QDomAttr attr = this->impl_.toAttr();
    return  std::make_shared<DomAttributeQt>(attr);
}
sp<IDomDocument>	DomDocumentQt::toDocument() const
{
    QDomDocument doc = this->impl_.toDocument();
    return  std::make_shared<DomDocumentQt>(doc);
}
sp<IDomElement>     DomDocumentQt::toElement() const
{
    QDomElement elem = this->impl_.toElement();
    return std::make_shared<DomElementQt>(elem);
}
std::string     DomDocumentQt::toText() const
{
    return this->impl_.toText().data().toStdString();
}

#endif

