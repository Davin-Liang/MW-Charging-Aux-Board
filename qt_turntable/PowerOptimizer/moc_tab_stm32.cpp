/****************************************************************************
** Meta object code from reading C++ file 'tab_stm32.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "tab_stm32.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tab_stm32.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TabSTM32_t {
    QByteArrayData data[13];
    char stringdata0[210];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TabSTM32_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TabSTM32_t qt_meta_stringdata_TabSTM32 = {
    {
QT_MOC_LITERAL(0, 0, 8), // "TabSTM32"
QT_MOC_LITERAL(1, 9, 35), // "on_pushButton_motor_control_c..."
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 34), // "on_pushButton_find_optimal_cl..."
QT_MOC_LITERAL(4, 81, 15), // "trajTypeChanged"
QT_MOC_LITERAL(5, 97, 5), // "index"
QT_MOC_LITERAL(6, 103, 19), // "onMotorDataReceived"
QT_MOC_LITERAL(7, 123, 11), // "MotorData_t"
QT_MOC_LITERAL(8, 135, 4), // "data"
QT_MOC_LITERAL(9, 140, 21), // "onChannelDataReceived"
QT_MOC_LITERAL(10, 162, 13), // "CurrentVPCh_t"
QT_MOC_LITERAL(11, 176, 20), // "onOptResDataReceived"
QT_MOC_LITERAL(12, 197, 12) // "OptResData_t"

    },
    "TabSTM32\0on_pushButton_motor_control_clicked\0"
    "\0on_pushButton_find_optimal_clicked\0"
    "trajTypeChanged\0index\0onMotorDataReceived\0"
    "MotorData_t\0data\0onChannelDataReceived\0"
    "CurrentVPCh_t\0onOptResDataReceived\0"
    "OptResData_t"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TabSTM32[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x0a /* Public */,
       4,    1,   46,    2, 0x0a /* Public */,
       6,    1,   49,    2, 0x0a /* Public */,
       9,    1,   52,    2, 0x0a /* Public */,
      11,    1,   55,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 10,    8,
    QMetaType::Void, 0x80000000 | 12,    8,

       0        // eod
};

void TabSTM32::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TabSTM32 *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_pushButton_motor_control_clicked(); break;
        case 1: _t->on_pushButton_find_optimal_clicked(); break;
        case 2: _t->trajTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onMotorDataReceived((*reinterpret_cast< const MotorData_t(*)>(_a[1]))); break;
        case 4: _t->onChannelDataReceived((*reinterpret_cast< const CurrentVPCh_t(*)>(_a[1]))); break;
        case 5: _t->onOptResDataReceived((*reinterpret_cast< const OptResData_t(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TabSTM32::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TabSTM32.data,
    qt_meta_data_TabSTM32,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TabSTM32::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabSTM32::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TabSTM32.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TabSTM32::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
