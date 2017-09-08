/****************************************************************************
** Meta object code from reading C++ file 'PlatformFileTest.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../sdk/rms_sdk/UnitTests/platform_ut/PlatformFileTest.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlatformFileTest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PlatformFileTest_t {
    QByteArrayData data[8];
    char stringdata0[92];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlatformFileTest_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlatformFileTest_t qt_meta_stringdata_PlatformFileTest = {
    {
QT_MOC_LITERAL(0, 0, 16), // "PlatformFileTest"
QT_MOC_LITERAL(1, 17, 8), // "testRead"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 9), // "testWrite"
QT_MOC_LITERAL(4, 37, 9), // "testClear"
QT_MOC_LITERAL(5, 47, 17), // "testReadAllAsText"
QT_MOC_LITERAL(6, 65, 14), // "testAppendText"
QT_MOC_LITERAL(7, 80, 11) // "testGetSize"

    },
    "PlatformFileTest\0testRead\0\0testWrite\0"
    "testClear\0testReadAllAsText\0testAppendText\0"
    "testGetSize"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlatformFileTest[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08 /* Private */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    0,   47,    2, 0x08 /* Private */,
       6,    0,   48,    2, 0x08 /* Private */,
       7,    0,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PlatformFileTest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlatformFileTest *_t = static_cast<PlatformFileTest *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->testRead(); break;
        case 1: _t->testWrite(); break;
        case 2: _t->testClear(); break;
        case 3: _t->testReadAllAsText(); break;
        case 4: _t->testAppendText(); break;
        case 5: _t->testGetSize(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject PlatformFileTest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PlatformFileTest.data,
      qt_meta_data_PlatformFileTest,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PlatformFileTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlatformFileTest::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PlatformFileTest.stringdata0))
        return static_cast<void*>(const_cast< PlatformFileTest*>(this));
    return QObject::qt_metacast(_clname);
}

int PlatformFileTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
