/****************************************************************************
** Meta object code from reading C++ file 'PlatformXmlTest.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../sdk/rms_sdk/UnitTests/platform_ut/PlatformXmlTest.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlatformXmlTest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PlatformXmlTest_t {
    QByteArrayData data[5];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlatformXmlTest_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlatformXmlTest_t qt_meta_stringdata_PlatformXmlTest = {
    {
QT_MOC_LITERAL(0, 0, 15), // "PlatformXmlTest"
QT_MOC_LITERAL(1, 16, 20), // "testSelectSingleNode"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 7), // "enabled"
QT_MOC_LITERAL(4, 46, 25) // "testSelectSingleNode_data"

    },
    "PlatformXmlTest\0testSelectSingleNode\0"
    "\0enabled\0testSelectSingleNode_data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlatformXmlTest[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x08 /* Private */,
       1,    0,   32,    2, 0x28 /* Private | MethodCloned */,
       4,    0,   33,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PlatformXmlTest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlatformXmlTest *_t = static_cast<PlatformXmlTest *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->testSelectSingleNode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->testSelectSingleNode(); break;
        case 2: _t->testSelectSingleNode_data(); break;
        default: ;
        }
    }
}

const QMetaObject PlatformXmlTest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PlatformXmlTest.data,
      qt_meta_data_PlatformXmlTest,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PlatformXmlTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlatformXmlTest::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PlatformXmlTest.stringdata0))
        return static_cast<void*>(const_cast< PlatformXmlTest*>(this));
    return QObject::qt_metacast(_clname);
}

int PlatformXmlTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
