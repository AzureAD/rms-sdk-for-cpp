/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IDOMDOCUMENT_H
#define IDOMDOCUMENT_H

#include <string>
#include "IDomNode.h"

class IDomElement;

class IDomDocument : public IDomNode
{
public:
    virtual sp<IDomElement>	documentElement() const = 0;
    virtual sp<IDomElement>	elementById(const std::string & elementId) = 0;
    virtual sp<DomNodeList> elementsByTagName(const std::string & tagname) const = 0;

    virtual bool setContent(const std::string & text, bool namespaceProcessing, std::string & errorMsg, int & errorLine, int & errorColumn) = 0;
    virtual bool setContent(const std::string & text, std::string & errorMsg, int & errorLine, int & errorColumn) = 0;

    virtual sp<IDomElement> SelectSingleNode(const std::string &xPath) = 0;
public:
    static sp<IDomDocument> create();
};

#endif // IDOMDOCUMENT_H
