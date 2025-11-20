/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "on_pushButton_connect_clicked",
    "",
    "on_pushButton_disconnect_clicked",
    "onSocketConnected",
    "onSocketDisconnected",
    "onSocketReadyRead",
    "onSocketError",
    "QAbstractSocket::SocketError",
    "error",
    "on_pushButton_motor_control_clicked",
    "on_pushButton_find_optimal_clicked",
    "on_pushButton_send_time_clicked",
    "on_textEdit_message_copyAvailable",
    "b",
    "on_textEdit_sd_data_copyAvailable",
    "initializeUIWithConfig",
    "on_traj_type_changed",
    "index",
    "onMotorDataReceived",
    "MotorData_t",
    "data",
    "onChannelDataReceived",
    "CurrentVPCh_t",
    "onOptResDataReceived",
    "OptResData_t"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[52];
    char stringdata0[11];
    char stringdata1[30];
    char stringdata2[1];
    char stringdata3[33];
    char stringdata4[18];
    char stringdata5[21];
    char stringdata6[18];
    char stringdata7[14];
    char stringdata8[29];
    char stringdata9[6];
    char stringdata10[36];
    char stringdata11[35];
    char stringdata12[32];
    char stringdata13[34];
    char stringdata14[2];
    char stringdata15[34];
    char stringdata16[23];
    char stringdata17[21];
    char stringdata18[6];
    char stringdata19[20];
    char stringdata20[12];
    char stringdata21[5];
    char stringdata22[22];
    char stringdata23[14];
    char stringdata24[21];
    char stringdata25[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 29),  // "on_pushButton_connect_clicked"
        QT_MOC_LITERAL(41, 0),  // ""
        QT_MOC_LITERAL(42, 32),  // "on_pushButton_disconnect_clicked"
        QT_MOC_LITERAL(75, 17),  // "onSocketConnected"
        QT_MOC_LITERAL(93, 20),  // "onSocketDisconnected"
        QT_MOC_LITERAL(114, 17),  // "onSocketReadyRead"
        QT_MOC_LITERAL(132, 13),  // "onSocketError"
        QT_MOC_LITERAL(146, 28),  // "QAbstractSocket::SocketError"
        QT_MOC_LITERAL(175, 5),  // "error"
        QT_MOC_LITERAL(181, 35),  // "on_pushButton_motor_control_c..."
        QT_MOC_LITERAL(217, 34),  // "on_pushButton_find_optimal_cl..."
        QT_MOC_LITERAL(252, 31),  // "on_pushButton_send_time_clicked"
        QT_MOC_LITERAL(284, 33),  // "on_textEdit_message_copyAvail..."
        QT_MOC_LITERAL(318, 1),  // "b"
        QT_MOC_LITERAL(320, 33),  // "on_textEdit_sd_data_copyAvail..."
        QT_MOC_LITERAL(354, 22),  // "initializeUIWithConfig"
        QT_MOC_LITERAL(377, 20),  // "on_traj_type_changed"
        QT_MOC_LITERAL(398, 5),  // "index"
        QT_MOC_LITERAL(404, 19),  // "onMotorDataReceived"
        QT_MOC_LITERAL(424, 11),  // "MotorData_t"
        QT_MOC_LITERAL(436, 4),  // "data"
        QT_MOC_LITERAL(441, 21),  // "onChannelDataReceived"
        QT_MOC_LITERAL(463, 13),  // "CurrentVPCh_t"
        QT_MOC_LITERAL(477, 20),  // "onOptResDataReceived"
        QT_MOC_LITERAL(498, 12)   // "OptResData_t"
    },
    "MainWindow",
    "on_pushButton_connect_clicked",
    "",
    "on_pushButton_disconnect_clicked",
    "onSocketConnected",
    "onSocketDisconnected",
    "onSocketReadyRead",
    "onSocketError",
    "QAbstractSocket::SocketError",
    "error",
    "on_pushButton_motor_control_clicked",
    "on_pushButton_find_optimal_clicked",
    "on_pushButton_send_time_clicked",
    "on_textEdit_message_copyAvailable",
    "b",
    "on_textEdit_sd_data_copyAvailable",
    "initializeUIWithConfig",
    "on_traj_type_changed",
    "index",
    "onMotorDataReceived",
    "MotorData_t",
    "data",
    "onChannelDataReceived",
    "CurrentVPCh_t",
    "onOptResDataReceived",
    "OptResData_t"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  110,    2, 0x08,    1 /* Private */,
       3,    0,  111,    2, 0x08,    2 /* Private */,
       4,    0,  112,    2, 0x08,    3 /* Private */,
       5,    0,  113,    2, 0x08,    4 /* Private */,
       6,    0,  114,    2, 0x08,    5 /* Private */,
       7,    1,  115,    2, 0x08,    6 /* Private */,
      10,    0,  118,    2, 0x08,    8 /* Private */,
      11,    0,  119,    2, 0x08,    9 /* Private */,
      12,    0,  120,    2, 0x08,   10 /* Private */,
      13,    1,  121,    2, 0x08,   11 /* Private */,
      15,    1,  124,    2, 0x08,   13 /* Private */,
      16,    0,  127,    2, 0x08,   15 /* Private */,
      17,    1,  128,    2, 0x08,   16 /* Private */,
      19,    1,  131,    2, 0x08,   18 /* Private */,
      22,    1,  134,    2, 0x08,   20 /* Private */,
      24,    1,  137,    2, 0x08,   22 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, 0x80000000 | 23,   21,
    QMetaType::Void, 0x80000000 | 25,   21,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'on_pushButton_connect_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButton_disconnect_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSocketConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSocketDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSocketReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSocketError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>,
        // method 'on_pushButton_motor_control_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButton_find_optimal_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButton_send_time_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_textEdit_message_copyAvailable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_textEdit_sd_data_copyAvailable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'initializeUIWithConfig'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_traj_type_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onMotorDataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const MotorData_t &, std::false_type>,
        // method 'onChannelDataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const CurrentVPCh_t &, std::false_type>,
        // method 'onOptResDataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const OptResData_t &, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_pushButton_connect_clicked(); break;
        case 1: _t->on_pushButton_disconnect_clicked(); break;
        case 2: _t->onSocketConnected(); break;
        case 3: _t->onSocketDisconnected(); break;
        case 4: _t->onSocketReadyRead(); break;
        case 5: _t->onSocketError((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 6: _t->on_pushButton_motor_control_clicked(); break;
        case 7: _t->on_pushButton_find_optimal_clicked(); break;
        case 8: _t->on_pushButton_send_time_clicked(); break;
        case 9: _t->on_textEdit_message_copyAvailable((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->on_textEdit_sd_data_copyAvailable((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->initializeUIWithConfig(); break;
        case 12: _t->on_traj_type_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->onMotorDataReceived((*reinterpret_cast< std::add_pointer_t<MotorData_t>>(_a[1]))); break;
        case 14: _t->onChannelDataReceived((*reinterpret_cast< std::add_pointer_t<CurrentVPCh_t>>(_a[1]))); break;
        case 15: _t->onOptResDataReceived((*reinterpret_cast< std::add_pointer_t<OptResData_t>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
