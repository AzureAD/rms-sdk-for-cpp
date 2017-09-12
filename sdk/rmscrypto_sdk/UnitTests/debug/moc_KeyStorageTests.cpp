/****************************************************************************
** Meta object code from reading C++ file 'KeyStorageTests.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../sdk/rmscrypto_sdk/UnitTests/KeyStorageTests.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KeyStorageTests.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_KeyStorageTests_t {
    QByteArrayData data[4];
    char stringdata0[40];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KeyStorageTests_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KeyStorageTests_t qt_meta_stringdata_KeyStorageTests = {
    {
QT_MOC_LITERAL(0, 0, 15), // "KeyStorageTests"
QT_MOC_LITERAL(1, 16, 13), // "KeyUsage_data"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 8) // "KeyUsage"

    },
    "KeyStorageTests\0KeyUsage_data\0\0KeyUsage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KeyStorageTests[] = {

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
       1,    0,   24,    2, 0x08 /* Private */,
       3,    0,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KeyStorageTests::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KeyStorageTests *_t = static_cast<KeyStorageTests *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->KeyUsage_data(); break;
        case 1: _t->KeyUsage(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KeyStorageTests::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KeyStorageTests.data,
      qt_meta_data_KeyStorageTests,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KeyStorageTests::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KeyStorageTests::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KeyStorageTests.stringdata0))
        return static_cast<void*>(const_cast< KeyStorageTests*>(this));
    return QObject::qt_metacast(_clname);
}

int KeyStorageTests::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
