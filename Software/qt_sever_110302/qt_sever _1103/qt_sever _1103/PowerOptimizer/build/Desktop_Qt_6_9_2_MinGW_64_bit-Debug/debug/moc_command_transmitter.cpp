/****************************************************************************
** Meta object code from reading C++ file 'command_transmitter.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../command_transmitter.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'command_transmitter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
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
struct qt_meta_tag_ZN18CommandTransmitterE_t {};
} // unnamed namespace

template <> constexpr inline auto CommandTransmitter::qt_create_metaobjectdata<qt_meta_tag_ZN18CommandTransmitterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
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

    QtMocHelpers::UintData qt_methods {
        // Signal 'motorDataReceived'
        QtMocHelpers::SignalData<void(const MotorData_t &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'channelDataReceived'
        QtMocHelpers::SignalData<void(const CurrentVPCh_t &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 4 },
        }}),
        // Signal 'optResDataReceived'
        QtMocHelpers::SignalData<void(const OptResData_t &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 4 },
        }}),
        // Slot 'on_new_connection'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_ready_read'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_disconnected'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_error_occurred'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CommandTransmitter, qt_meta_tag_ZN18CommandTransmitterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CommandTransmitter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18CommandTransmitterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18CommandTransmitterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18CommandTransmitterE_t>.metaTypes,
    nullptr
} };

void CommandTransmitter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CommandTransmitter *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
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
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CommandTransmitter::*)(const MotorData_t & )>(_a, &CommandTransmitter::motorDataReceived, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (CommandTransmitter::*)(const CurrentVPCh_t & )>(_a, &CommandTransmitter::channelDataReceived, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (CommandTransmitter::*)(const OptResData_t & )>(_a, &CommandTransmitter::optResDataReceived, 2))
            return;
    }
}

const QMetaObject *CommandTransmitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CommandTransmitter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18CommandTransmitterE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CommandTransmitter::motorDataReceived(const MotorData_t & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void CommandTransmitter::channelDataReceived(const CurrentVPCh_t & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void CommandTransmitter::optResDataReceived(const OptResData_t & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
