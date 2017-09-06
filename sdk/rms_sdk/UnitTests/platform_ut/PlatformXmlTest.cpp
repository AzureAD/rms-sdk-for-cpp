/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PlatformXmlTest.h"
#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Xml/IDomDocument.h"
#include "../../Platform/Xml/IDomNode.h"
#include "../../Platform/Xml/IDomElement.h"

using namespace rmscore::platform::logger;


PlatformXmlTest::PlatformXmlTest()
{
}
void PlatformXmlTest::testSelectSingleNode_data()
{
    QTest::addColumn<QString>("xmlAsString");
    QTest::addColumn<QString>("xPathRequest");
    QTest::addColumn<QString>("expectedResult");

    QString path1 = QString(SRCDIR) + "data/testXPath1.xml";
    QFile file1(path1);
    QVERIFY(file1.open(QIODevice::ReadOnly | QIODevice::Text));
    QTest::newRow("xpath1")
        << QString(file1.readAll())
        << "kml/Document/Placemark[last()]/GeometryCollection/LineString/coordinates"
        << "0.000010,0.000020,0.000030";

    QString path2 = QString(SRCDIR) + "data/testXPath2.xml";
    QFile file2(path2);
    QVERIFY(file2.open(QIODevice::ReadOnly | QIODevice::Text));
    auto file2AsString = QString(file2.readAll());
    QTest::newRow("xpath2")
        << file2AsString
        << "bookstore/book/author[last-name = \"Bob\" and first-name = \"Joe\"]/award"
        << "Trenton Literary Review Honorable Mention";
}
void PlatformXmlTest::testSelectSingleNode(bool enabled)
{
    if (!enabled) return;

    auto doc = IDomDocument::create();
    std::string errorMsg;
    int errorLine = 0;
    int errorColumn = 0;

    QFETCH(QString, xmlAsString);
    QFETCH(QString, xPathRequest);
    QFETCH(QString, expectedResult);

    auto ok = doc->setContent(xmlAsString.toStdString(), errorMsg, errorLine, errorColumn);
    QVERIFY2(ok, errorMsg.data());

    auto pnode = doc->SelectSingleNode(xPathRequest.toStdString());
    auto realResult = pnode->toElement()->text();
    Logger::Hidden("expc: %s", expectedResult.toStdString().data());
    Logger::Hidden("real: %s", realResult.data());
    QVERIFY(realResult == expectedResult.toStdString());
}
