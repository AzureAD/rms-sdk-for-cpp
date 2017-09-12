/****************************************************************************
** Meta object code from reading C++ file 'LicenseParserTest.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../sdk/rms_sdk/UnitTests/rest_clients_ut/LicenseParserTest.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LicenseParserTest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LicenseParserTest_t {
    QByteArrayData data[4];
    char stringdata0[58];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LicenseParserTest_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LicenseParserTest_t qt_meta_stringdata_LicenseParserTest = {
    {
QT_MOC_LITERAL(0, 0, 17), // "LicenseParserTest"
QT_MOC_LITERAL(1, 18, 17), // "test_UTF8_License"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 20) // "test_UTF16LE_License"

    },
    "LicenseParserTest\0test_UTF8_License\0"
    "\0test_UTF16LE_License"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LicenseParserTest[] = {

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

void LicenseParserTest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LicenseParserTest *_t = static_cast<LicenseParserTest *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->test_UTF8_License(); break;
        case 1: _t->test_UTF16LE_License(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject LicenseParserTest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_LicenseParserTest.data,
      qt_meta_data_LicenseParserTest,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LicenseParserTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LicenseParserTest::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LicenseParserTest.stringdata0))
        return static_cast<void*>(const_cast< LicenseParserTest*>(this));
    return QObject::qt_metacast(_clname);
}

int LicenseParserTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
