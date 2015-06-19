/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PLATFORMJSONOBJECTTEST
#define PLATFORMJSONOBJECTTEST
#include <QtTest>

class PlatformJsonObjectTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testGetNamedString(bool enabled = true);
    void testGetNamedString_data();

    void testGetNamedBool(bool enabled = true);
    void testGetNamedBool_data();

//    void testGetNamedNumber(bool enabled = true);
//    void testGetNamedNumber_data();

    void testGetNamedObject(bool enabled = true);
    void testGetNamedObject_data();

//    void testGetNamedArray();
//    void testGetNamedArray_data();

//    void testToStringDictionary();
//    void testToStringDictionary_data();

    void testStringify();
    void testStringify_data();

    void testSetNamedString();
//    void testSetNamedBool();
//    void testSetNamedNumber();
    void testSetNamedObject();
    void testSetNamedArray();
};
#endif // PLATFORMJSONOBJECTTEST

