/****************************************************************************
** Meta object code from reading C++ file 'PlatformJsonObjectTest.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../sdk/rms_sdk/UnitTests/platform_ut/PlatformJsonObjectTest.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlatformJsonObjectTest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PlatformJsonObjectTest_t {
    QByteArrayData data[14];
    char stringdata0[246];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlatformJsonObjectTest_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlatformJsonObjectTest_t qt_meta_stringdata_PlatformJsonObjectTest = {
    {
QT_MOC_LITERAL(0, 0, 22), // "PlatformJsonObjectTest"
QT_MOC_LITERAL(1, 23, 18), // "testGetNamedString"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 7), // "enabled"
QT_MOC_LITERAL(4, 51, 23), // "testGetNamedString_data"
QT_MOC_LITERAL(5, 75, 16), // "testGetNamedBool"
QT_MOC_LITERAL(6, 92, 21), // "testGetNamedBool_data"
QT_MOC_LITERAL(7, 114, 18), // "testGetNamedObject"
QT_MOC_LITERAL(8, 133, 23), // "testGetNamedObject_data"
QT_MOC_LITERAL(9, 157, 13), // "testStringify"
QT_MOC_LITERAL(10, 171, 18), // "testStringify_data"
QT_MOC_LITERAL(11, 190, 18), // "testSetNamedString"
QT_MOC_LITERAL(12, 209, 18), // "testSetNamedObject"
QT_MOC_LITERAL(13, 228, 17) // "testSetNamedArray"

    },
    "PlatformJsonObjectTest\0testGetNamedString\0"
    "\0enabled\0testGetNamedString_data\0"
    "testGetNamedBool\0testGetNamedBool_data\0"
    "testGetNamedObject\0testGetNamedObject_data\0"
    "testStringify\0testStringify_data\0"
    "testSetNamedString\0testSetNamedObject\0"
    "testSetNamedArray"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlatformJsonObjectTest[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x08 /* Private */,
       1,    0,   87,    2, 0x28 /* Private | MethodCloned */,
       4,    0,   88,    2, 0x08 /* Private */,
       5,    1,   89,    2, 0x08 /* Private */,
       5,    0,   92,    2, 0x28 /* Private | MethodCloned */,
       6,    0,   93,    2, 0x08 /* Private */,
       7,    1,   94,    2, 0x08 /* Private */,
       7,    0,   97,    2, 0x28 /* Private | MethodCloned */,
       8,    0,   98,    2, 0x08 /* Private */,
       9,    0,   99,    2, 0x08 /* Private */,
      10,    0,  100,    2, 0x08 /* Private */,
      11,    0,  101,    2, 0x08 /* Private */,
      12,    0,  102,    2, 0x08 /* Private */,
      13,    0,  103,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PlatformJsonObjectTest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlatformJsonObjectTest *_t = static_cast<PlatformJsonObjectTest *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->testGetNamedString((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->testGetNamedString(); break;
        case 2: _t->testGetNamedString_data(); break;
        case 3: _t->testGetNamedBool((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->testGetNamedBool(); break;
        case 5: _t->testGetNamedBool_data(); break;
        case 6: _t->testGetNamedObject((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->testGetNamedObject(); break;
        case 8: _t->testGetNamedObject_data(); break;
        case 9: _t->testStringify(); break;
        case 10: _t->testStringify_data(); break;
        case 11: _t->testSetNamedString(); break;
        case 12: _t->testSetNamedObject(); break;
        case 13: _t->testSetNamedArray(); break;
        default: ;
        }
    }
}

const QMetaObject PlatformJsonObjectTest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PlatformJsonObjectTest.data,
      qt_meta_data_PlatformJsonObjectTest,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PlatformJsonObjectTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlatformJsonObjectTest::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PlatformJsonObjectTest.stringdata0))
        return static_cast<void*>(const_cast< PlatformJsonObjectTest*>(this));
    return QObject::qt_metacast(_clname);
}

int PlatformJsonObjectTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
