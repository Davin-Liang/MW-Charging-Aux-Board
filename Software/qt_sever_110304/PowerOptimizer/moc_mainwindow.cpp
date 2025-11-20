/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[28];
    char stringdata0[590];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 29), // "on_pushButton_connect_clicked"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 32), // "on_pushButton_disconnect_clicked"
QT_MOC_LITERAL(4, 75, 17), // "onSocketConnected"
QT_MOC_LITERAL(5, 93, 20), // "onSocketDisconnected"
QT_MOC_LITERAL(6, 114, 17), // "onSocketReadyRead"
QT_MOC_LITERAL(7, 132, 13), // "onSocketError"
QT_MOC_LITERAL(8, 146, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(9, 175, 5), // "error"
QT_MOC_LITERAL(10, 181, 35), // "on_pushButton_motor_control_c..."
QT_MOC_LITERAL(11, 217, 34), // "on_pushButton_find_optimal_cl..."
QT_MOC_LITERAL(12, 252, 33), // "on_textEdit_message_copyAvail..."
QT_MOC_LITERAL(13, 286, 1), // "b"
QT_MOC_LITERAL(14, 288, 33), // "on_textEdit_sd_data_copyAvail..."
QT_MOC_LITERAL(15, 322, 22), // "initializeUIWithConfig"
QT_MOC_LITERAL(16, 345, 20), // "on_traj_type_changed"
QT_MOC_LITERAL(17, 366, 5), // "index"
QT_MOC_LITERAL(18, 372, 19), // "onMotorDataReceived"
QT_MOC_LITERAL(19, 392, 11), // "MotorData_t"
QT_MOC_LITERAL(20, 404, 4), // "data"
QT_MOC_LITERAL(21, 409, 21), // "onChannelDataReceived"
QT_MOC_LITERAL(22, 431, 13), // "CurrentVPCh_t"
QT_MOC_LITERAL(23, 445, 20), // "onOptResDataReceived"
QT_MOC_LITERAL(24, 466, 12), // "OptResData_t"
QT_MOC_LITERAL(25, 479, 37), // "on_pushButton_filenamedisplay..."
QT_MOC_LITERAL(26, 517, 31), // "on_pushButton_read_file_clicked"
QT_MOC_LITERAL(27, 549, 40) // "on_listWidget_files_itemSelec..."

    },
    "MainWindow\0on_pushButton_connect_clicked\0"
    "\0on_pushButton_disconnect_clicked\0"
    "onSocketConnected\0onSocketDisconnected\0"
    "onSocketReadyRead\0onSocketError\0"
    "QAbstractSocket::SocketError\0error\0"
    "on_pushButton_motor_control_clicked\0"
    "on_pushButton_find_optimal_clicked\0"
    "on_textEdit_message_copyAvailable\0b\0"
    "on_textEdit_sd_data_copyAvailable\0"
    "initializeUIWithConfig\0on_traj_type_changed\0"
    "index\0onMotorDataReceived\0MotorData_t\0"
    "data\0onChannelDataReceived\0CurrentVPCh_t\0"
    "onOptResDataReceived\0OptResData_t\0"
    "on_pushButton_filenamedisplay_clicked\0"
    "on_pushButton_read_file_clicked\0"
    "on_listWidget_files_itemSelectionChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  104,    2, 0x08 /* Private */,
       3,    0,  105,    2, 0x08 /* Private */,
       4,    0,  106,    2, 0x08 /* Private */,
       5,    0,  107,    2, 0x08 /* Private */,
       6,    0,  108,    2, 0x08 /* Private */,
       7,    1,  109,    2, 0x08 /* Private */,
      10,    0,  112,    2, 0x08 /* Private */,
      11,    0,  113,    2, 0x08 /* Private */,
      12,    1,  114,    2, 0x08 /* Private */,
      14,    1,  117,    2, 0x08 /* Private */,
      15,    0,  120,    2, 0x08 /* Private */,
      16,    1,  121,    2, 0x08 /* Private */,
      18,    1,  124,    2, 0x08 /* Private */,
      21,    1,  127,    2, 0x08 /* Private */,
      23,    1,  130,    2, 0x08 /* Private */,
      25,    0,  133,    2, 0x08 /* Private */,
      26,    0,  134,    2, 0x08 /* Private */,
      27,    0,  135,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void, 0x80000000 | 22,   20,
    QMetaType::Void, 0x80000000 | 24,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

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
        case 5: _t->onSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 6: _t->on_pushButton_motor_control_clicked(); break;
        case 7: _t->on_pushButton_find_optimal_clicked(); break;
        case 8: _t->on_textEdit_message_copyAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->on_textEdit_sd_data_copyAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->initializeUIWithConfig(); break;
        case 11: _t->on_traj_type_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->onMotorDataReceived((*reinterpret_cast< const MotorData_t(*)>(_a[1]))); break;
        case 13: _t->onChannelDataReceived((*reinterpret_cast< const CurrentVPCh_t(*)>(_a[1]))); break;
        case 14: _t->onOptResDataReceived((*reinterpret_cast< const OptResData_t(*)>(_a[1]))); break;
        case 15: _t->on_pushButton_filenamedisplay_clicked(); break;
        case 16: _t->on_pushButton_read_file_clicked(); break;
        case 17: _t->on_listWidget_files_itemSelectionChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
