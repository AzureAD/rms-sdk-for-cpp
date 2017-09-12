/****************************************************************************
** Meta object code from reading C++ file 'PlatformHttpClientTest.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../sdk/rms_sdk/UnitTests/platform_ut/PlatformHttpClientTest.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlatformHttpClientTest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PlatformHttpClientTest_t {
    QByteArrayData data[4];
    char stringdata0[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlatformHttpClientTest_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlatformHttpClientTest_t qt_meta_stringdata_PlatformHttpClientTest = {
    {
QT_MOC_LITERAL(0, 0, 22), // "PlatformHttpClientTest"
QT_MOC_LITERAL(1, 23, 14), // "testHttpClient"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 7) // "enabled"

    },
    "PlatformHttpClientTest\0testHttpClient\0"
    "\0enabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlatformHttpClientTest[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x08 /* Private */,
       1,    0,   27,    2, 0x28 /* Private | MethodCloned */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

       0        // eod
};

void PlatformHttpClientTest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlatformHttpClientTest *_t = static_cast<PlatformHttpClientTest *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->testHttpClient((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->testHttpClient(); break;
        default: ;
        }
    }
}

const QMetaObject PlatformHttpClientTest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PlatformHttpClientTest.data,
      qt_meta_data_PlatformHttpClientTest,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PlatformHttpClientTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlatformHttpClientTest::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PlatformHttpClientTest.stringdata0))
        return static_cast<void*>(const_cast< PlatformHttpClientTest*>(this));
    return QObject::qt_metacast(_clname);
}

int PlatformHttpClientTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
