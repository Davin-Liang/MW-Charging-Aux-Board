/****************************************************************************
** Meta object code from reading C++ file 'command_transmitter.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../command_transmitter.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'command_transmitter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSCommandTransmitterENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSCommandTransmitterENDCLASS = QtMocHelpers::stringData(
    "CommandTransmitter",
    "motorDataReceived",
    "",
    "MotorData_t",
    "data",
    "channelDataReceived",
    "CurrentVPCh_t",
    "optResDataReceived",
    "OptResData_t",
    "on_new_connection",
    "on_ready_read",
    "on_disconnected",
    "on_error_occurred",
    "QAbstractSocket::SocketError",
    "error"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSCommandTransmitterENDCLASS_t {
    uint offsetsAndSizes[30];
    char stringdata0[19];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[5];
    char stringdata5[20];
    char stringdata6[14];
    char stringdata7[19];
    char stringdata8[13];
    char stringdata9[18];
    char stringdata10[14];
    char stringdata11[16];
    char stringdata12[18];
    char stringdata13[29];
    char stringdata14[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSCommandTransmitterENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSCommandTransmitterENDCLASS_t qt_meta_stringdata_CLASSCommandTransmitterENDCLASS = {
    {
        QT_MOC_LITERAL(0, 18),  // "CommandTransmitter"
        QT_MOC_LITERAL(19, 17),  // "motorDataReceived"
        QT_MOC_LITERAL(37, 0),  // ""
        QT_MOC_LITERAL(38, 11),  // "MotorData_t"
        QT_MOC_LITERAL(50, 4),  // "data"
        QT_MOC_LITERAL(55, 19),  // "channelDataReceived"
        QT_MOC_LITERAL(75, 13),  // "CurrentVPCh_t"
        QT_MOC_LITERAL(89, 18),  // "optResDataReceived"
        QT_MOC_LITERAL(108, 12),  // "OptResData_t"
        QT_MOC_LITERAL(121, 17),  // "on_new_connection"
        QT_MOC_LITERAL(139, 13),  // "on_ready_read"
        QT_MOC_LITERAL(153, 15),  // "on_disconnected"
        QT_MOC_LITERAL(169, 17),  // "on_error_occurred"
        QT_MOC_LITERAL(187, 28),  // "QAbstractSocket::SocketError"
        QT_MOC_LITERAL(216, 5)   // "error"
    },
    "CommandTransmitter",
    "motorDataReceived",
    "",
    "MotorData_t",
    "data",
    "channelDataReceived",
    "CurrentVPCh_t",
    "optResDataReceived",
    "OptResData_t",
    "on_new_connection",
    "on_ready_read",
    "on_disconnected",
    "on_error_occurred",
    "QAbstractSocket::SocketError",
    "error"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCommandTransmitterENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   56,    2, 0x06,    1 /* Public */,
       5,    1,   59,    2, 0x06,    3 /* Public */,
       7,    1,   62,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,   65,    2, 0x08,    7 /* Private */,
      10,    0,   66,    2, 0x08,    8 /* Private */,
      11,    0,   67,    2, 0x08,    9 /* Private */,
      12,    1,   68,    2, 0x08,   10 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    4,
    QMetaType::Void, 0x80000000 | 8,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject CommandTransmitter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSCommandTransmitterENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCommandTransmitterENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCommandTransmitterENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CommandTransmitter, std::true_type>,
        // method 'motorDataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const MotorData_t &, std::false_type>,
        // method 'channelDataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const CurrentVPCh_t &, std::false_type>,
        // method 'optResDataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const OptResData_t &, std::false_type>,
        // method 'on_new_connection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ready_read'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_disconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_error_occurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>
    >,
    nullptr
} };

void CommandTransmitter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CommandTransmitter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->motorDataReceived((*reinterpret_cast< std::add_pointer_t<MotorData_t>>(_a[1]))); break;
        case 1: _t->channelDataReceived((*reinterpret_cast< std::add_pointer_t<CurrentVPCh_t>>(_a[1]))); break;
        case 2: _t->optResDataReceived((*reinterpret_cast< std::add_pointer_t<OptResData_t>>(_a[1]))); break;
        case 3: _t->on_new_connection(); break;
        case 4: _t->on_ready_read(); break;
        case 5: _t->on_disconnected(); break;
        case 6: _t->on_error_occurred((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CommandTransmitter::*)(const MotorData_t & );
            if (_t _q_method = &CommandTransmitter::motorDataReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CommandTransmitter::*)(const CurrentVPCh_t & );
            if (_t _q_method = &CommandTransmitter::channelDataReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CommandTransmitter::*)(const OptResData_t & );
            if (_t _q_method = &CommandTransmitter::optResDataReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *CommandTransmitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CommandTransmitter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCommandTransmitterENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CommandTransmitter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CommandTransmitter::motorDataReceived(const MotorData_t & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CommandTransmitter::channelDataReceived(const CurrentVPCh_t & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CommandTransmitter::optResDataReceived(const OptResData_t & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
